#include "photoframe.h"
#include "playlistmanager.h"

#include <QDateTime>
#include <QDirIterator>
#include <QImageReader>
#include <QtConcurrent>
#include <QProcess>
#include <QCoreApplication>
#include <QQmlEngine>
#include <algorithm>
#include <random>

// --- ImageProvider ---

ImageProvider::ImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image) {}

QImage ImageProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize) {
    Q_UNUSED(id);
    QMutexLocker lock(&m_mutex);
    if (size) *size = m_current.size();
    if (requestedSize.isValid())
        return m_current.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    return m_current;
}

void ImageProvider::setCurrentImage(const QImage& img) {
    QMutexLocker lock(&m_mutex);
    m_current = img;
}

// --- PhotoFrameBackend ---

PhotoFrameBackend::PhotoFrameBackend(QObject* parent)
    : QObject(parent)
{
    m_config.load();
    qInfo() << "Config loaded: server=" << m_config.server << "share=" << m_config.share
             << "useRtsp=" << m_config.useRtsp << "rtspUrl=" << m_config.rtspUrl
             << "useGuest=" << m_config.useGuest;

    m_tickTimer = new QTimer(this);
    connect(m_tickTimer, &QTimer::timeout, this, &PhotoFrameBackend::onTick);
    m_tickTimer->start(1000);

    m_slideshowTimer = new QTimer(this);
    connect(m_slideshowTimer, &QTimer::timeout, this, &PhotoFrameBackend::nextSlide);

    m_rtspRetryTimer = new QTimer(this);
    m_rtspRetryTimer->setSingleShot(true);
    connect(m_rtspRetryTimer, &QTimer::timeout, this, &PhotoFrameBackend::onRtspRetryTimeout);

    m_rtspFallbackTimer = new QTimer(this);
    m_rtspFallbackTimer->setSingleShot(true);
    connect(m_rtspFallbackTimer, &QTimer::timeout, this, &PhotoFrameBackend::onRtspFallbackTimeout);

    // Defer all startup to after QML loads — signals need listeners
    QTimer::singleShot(500, this, [this]() {
        m_playlist = PlaylistManager::load();
        if (!m_playlist.isEmpty()) {
            qInfo() << "Loaded" << m_playlist.size() << "cached files";
            if (m_config.shuffle)
                std::shuffle(m_playlist.begin(), m_playlist.end(), std::mt19937(std::random_device()()));
            checkSchedule();
            if (!m_isSleeping) {
                m_slideshowTimer->start(m_config.interval);
                nextSlide();
            }
        }
        // Always mount SMB / scan — needed so cached paths are accessible
        // even if playlist was loaded from cache (SMB may not be mounted yet).
        // When scan finishes it will replace the playlist and start the slideshow.
        QTimer::singleShot(1000, this, &PhotoFrameBackend::connectAndScan);
    });
}

PhotoFrameBackend::~PhotoFrameBackend() {
    m_destroyed = true;
    m_tickTimer->stop();
    m_slideshowTimer->stop();
}

QString PhotoFrameBackend::currentTime() const {
    return QTime::currentTime().toString("HH:mm");
}

QString PhotoFrameBackend::currentDate() const {
    return QDate::currentDate().toString("dd.MM");
}

QString PhotoFrameBackend::currentImagePath() const {
    return m_currentImagePath;
}

int PhotoFrameBackend::pageIndex() const { return m_pageIndex; }
void PhotoFrameBackend::setPageIndex(int p) {
    if (m_pageIndex != p) { m_pageIndex = p; emit pageIndexChanged(); }
}

// Settings getters
QString PhotoFrameBackend::server() const { return m_config.server; }
QString PhotoFrameBackend::share() const { return m_config.share; }
QString PhotoFrameBackend::user() const { return m_config.user; }
QString PhotoFrameBackend::pass() const { return m_config.pass; }
int PhotoFrameBackend::interval() const { return m_config.interval / 1000; }
bool PhotoFrameBackend::shuffle() const { return m_config.shuffle; }
bool PhotoFrameBackend::useSchedule() const { return m_config.useSchedule; }
QString PhotoFrameBackend::wakeTimeStr() const { return m_config.wakeTime.toString("HH:mm"); }
QString PhotoFrameBackend::sleepTimeStr() const { return m_config.sleepTime.toString("HH:mm"); }
bool PhotoFrameBackend::useGuest() const { return m_config.useGuest; }
QString PhotoFrameBackend::smbVers() const { return m_config.smbVers; }
bool PhotoFrameBackend::useRtsp() const { return m_config.useRtsp; }
QString PhotoFrameBackend::rtspUrl() const { return m_config.rtspUrl; }

int PhotoFrameBackend::rtspState() const { return m_rtspState; }
QString PhotoFrameBackend::rtspErrorMsg() const { return m_rtspErrorMsg; }

// Settings setters
void PhotoFrameBackend::setServer(const QString& v) { if (m_config.server != v) { m_config.server = v; emit configChanged(); } }
void PhotoFrameBackend::setShare(const QString& v) { if (m_config.share != v) { m_config.share = v; emit configChanged(); } }
void PhotoFrameBackend::setUser(const QString& v) { if (m_config.user != v) { m_config.user = v; emit configChanged(); } }
void PhotoFrameBackend::setPass(const QString& v) { if (m_config.pass != v) { m_config.pass = v; emit configChanged(); } }
void PhotoFrameBackend::setInterval(int v) { int ms = v * 1000; if (m_config.interval != ms) { m_config.interval = ms; emit configChanged(); } }
void PhotoFrameBackend::setShuffle(bool v) { if (m_config.shuffle != v) { m_config.shuffle = v; emit configChanged(); } }
void PhotoFrameBackend::setUseSchedule(bool v) { if (m_config.useSchedule != v) { m_config.useSchedule = v; emit configChanged(); } }
void PhotoFrameBackend::setWakeTimeStr(const QString& v) {
    QTime t = QTime::fromString(v, "HH:mm");
    if (t.isValid() && m_config.wakeTime != t) { m_config.wakeTime = t; emit configChanged(); }
}
void PhotoFrameBackend::setSleepTimeStr(const QString& v) {
    QTime t = QTime::fromString(v, "HH:mm");
    if (t.isValid() && m_config.sleepTime != t) { m_config.sleepTime = t; emit configChanged(); }
}
void PhotoFrameBackend::setUseGuest(bool v) { if (m_config.useGuest != v) { m_config.useGuest = v; emit configChanged(); } }
void PhotoFrameBackend::setSmbVers(const QString& v) { if (m_config.smbVers != v) { m_config.smbVers = v; emit configChanged(); } }
void PhotoFrameBackend::setUseRtsp(bool v) { if (m_config.useRtsp != v) { m_config.useRtsp = v; emit configChanged(); } }
void PhotoFrameBackend::setRtspUrl(const QString& v) { if (m_config.rtspUrl != v) { m_config.rtspUrl = v; emit configChanged(); } }

void PhotoFrameBackend::onTick() {
    emit tick();
    checkSchedule();
}

void PhotoFrameBackend::checkSchedule() {
    if (!m_config.useSchedule) return;
    QTime now = QTime::currentTime();
    bool awake = m_config.wakeTime <= m_config.sleepTime
        ? (now >= m_config.wakeTime && now < m_config.sleepTime)
        : (now >= m_config.wakeTime || now < m_config.sleepTime);
    if (awake && m_isSleeping) setSleepMode(false);
    else if (!awake && !m_isSleeping) setSleepMode(true);
}

void PhotoFrameBackend::setSleepMode(bool sleep) {
    m_isSleeping = sleep;
    if (sleep) {
        m_slideshowTimer->stop();
    } else {
        if (!m_playlist.isEmpty()) {
            m_slideshowTimer->start(m_config.interval);
            nextSlide();
        }
    }
    emit sleepChanged(sleep);
}

void PhotoFrameBackend::nextSlide() {
    if (m_playlist.isEmpty() || m_isSleeping) {
        qInfo() << "nextSlide skipped: playlist=" << m_playlist.size() << "sleeping=" << m_isSleeping;
        return;
    }
    int startIdx = m_idx;
    int playlistSize = m_playlist.size();
    QStringList playlist = m_playlist; // snapshot — safe to read from worker
    qInfo() << "nextSlide: idx=" << startIdx << "of" << playlistSize;

    QtConcurrent::run([this, startIdx, playlistSize, playlist]() {
        int idx = startIdx;
        QImage loaded;
        QString loadedPath;

        // Skip broken files directly in the worker thread
        for (int attempt = 0; attempt < kMaxConsecutiveFails; ++attempt) {
            loadedPath = playlist[idx];
            QImageReader reader(loadedPath);
            reader.setAutoTransform(true);
            loaded = reader.read();
            if (!loaded.isNull()) break;
            idx = (idx + 1) % playlistSize;
        }

        if (m_destroyed) return;

        if (loaded.isNull()) {
            m_idx = (idx + 1) % playlistSize;
            return;
        }

        m_idx = (idx + 1) % playlistSize;
        m_imageProvider->setCurrentImage(loaded);
        m_imageCounter++;
        m_currentImagePath = QString("image://current/img?id=%1").arg(m_imageCounter);
        QMetaObject::invokeMethod(this, [this]() {
            qInfo() << "Image loaded successfully, counter=" << m_imageCounter;
            emit imageChanged();
        });
    });
}

void PhotoFrameBackend::prevSlide() {
    if (m_isSleeping || m_playlist.isEmpty()) return;
    // nextSlide() reads m_idx then does +1, so we need -2 to go back one
    m_idx = (m_idx - 2 + m_playlist.size()) % m_playlist.size();
    nextSlide();
}

void PhotoFrameBackend::toggleSlideshow() {
    if (m_slideshowTimer->isActive())
        m_slideshowTimer->stop();
    else if (!m_playlist.isEmpty() && !m_isSleeping)
        m_slideshowTimer->start(m_config.interval);
}

void PhotoFrameBackend::saveSettings() {
    qInfo() << "Saving settings: server=" << m_config.server << "useRtsp=" << m_config.useRtsp;
    m_config.save();
    PlaylistManager::clear();
    stopRtsp();

    connectAndScan();
    setPageIndex(0);
}

void PhotoFrameBackend::connectAndScan() {
    qInfo() << "connectAndScan: server=" << m_config.server << "scanning=" << m_scanning;
    if (m_scanning) {
        qInfo() << "Already scanning, skipping";
        return;
    }
    if (m_config.server.isEmpty()) {
        qInfo() << "Server empty, showing settings";
        setPageIndex(1);
        return;
    }
    m_scanning = true;
    QString server = m_config.server;
    QString share = m_config.share;
    QString user = m_config.user;
    QString pass = m_config.pass;
    QString smbVers = m_config.smbVers;
    bool useGuest = m_config.useGuest;

    QtConcurrent::run([this, server, share, user, pass, smbVers, useGuest]() {
#ifdef Q_OS_WIN
        QString path = QString(R"(\\%1\%2)").arg(server, share);
        if (useGuest) {
            QProcess::execute("net", {"use", path});
        } else {
            QProcess::execute("net", {"use", path, pass, QString("/user:%1").arg(user)});
        }
#else
        QDir().mkpath("/mnt/photoframe");
        QString opts = QString("vers=%1").arg(smbVers);
        if (!useGuest) {
            opts += QString(",username=%1,password=%2").arg(user, pass);
        }
        QProcess::execute("mount", {"-t", "cifs", QString("//%1/%2").arg(server, share), "/mnt/photoframe", "-o", opts});
#endif
        if (m_destroyed) return;
        QString scanPath =
#ifdef Q_OS_WIN
            QString(R"(\\%1\%2)").arg(server, share);
#else
            "/mnt/photoframe";
#endif
        QStringList files;
        QDirIterator it(scanPath, {"*.jpg", "*.jpeg", "*.png"}, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) files << it.next();
        if (m_destroyed) return;
        QMetaObject::invokeMethod(this, "onScanFinished", Q_ARG(QStringList, files));
    });
}

void PhotoFrameBackend::startRtsp() {
    if (!m_config.useRtsp || m_config.rtspUrl.isEmpty()) return;
    m_slideshowTimer->stop();
    m_rtspRetryCount = 0;
    m_rtspRetryTimer->stop();
    m_rtspFallbackTimer->stop();
    setRtspState(RtspConnecting);
    m_rtspErrorMsg = QString::fromUtf8("\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u0435...");
    emit rtspErrorMsgChanged();
    emit rtspShowOverlay(m_rtspErrorMsg);
    m_rtspFallbackTimer->start(5000);
    emit rtspPlay(m_config.rtspUrl);
}

void PhotoFrameBackend::stopRtsp() {
    m_rtspRetryTimer->stop();
    m_rtspFallbackTimer->stop();
    m_rtspRetryCount = 0;
    setRtspState(RtspIdle);
    emit rtspStopPlayer();
    emit rtspHideOverlay();
}

void PhotoFrameBackend::reconnectRtsp() {
    stopRtsp();
    startRtsp();
}

void PhotoFrameBackend::setRtspState(RtspState s) {
    if (m_rtspState != s) {
        m_rtspState = s;
        emit rtspStateChanged();
    }
}

void PhotoFrameBackend::onRtspPlaying() {
    m_rtspFallbackTimer->stop();
    m_rtspRetryTimer->stop();
    m_rtspRetryCount = 0;
    setRtspState(RtspPlaying);
    // Hide overlay after 5 seconds
    QTimer::singleShot(5000, this, [this]() {
        if (m_rtspState == RtspPlaying)
            emit rtspHideOverlay();
    });
}

void PhotoFrameBackend::onRtspError(const QString& msg) {
    if (m_rtspState == RtspIdle) return; // already stopped
    m_rtspFallbackTimer->stop();
    m_rtspRetryCount++;

    if (m_rtspRetryCount <= kMaxRtspRetries) {
        m_rtspErrorMsg = msg + QString::fromUtf8(" \u2014 \u043f\u043e\u0432\u0442\u043e\u0440 %1/%2").arg(m_rtspRetryCount).arg(kMaxRtspRetries);
        emit rtspErrorMsgChanged();
        emit rtspShowOverlay(m_rtspErrorMsg);
        setRtspState(RtspError);
        m_rtspRetryTimer->start(2000);
    } else {
        onRtspFallbackTimeout();
    }
}

void PhotoFrameBackend::onRtspRetryTimeout() {
    if (m_rtspState == RtspIdle) return;
    m_rtspErrorMsg = QString::fromUtf8("\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u0435... \u043f\u043e\u0432\u0442\u043e\u0440 %1/%2").arg(m_rtspRetryCount + 1).arg(kMaxRtspRetries);
    emit rtspErrorMsgChanged();
    emit rtspShowOverlay(m_rtspErrorMsg);
    setRtspState(RtspConnecting);
    emit rtspPlay(m_config.rtspUrl);
    // Reset fallback timer on retry
    m_rtspFallbackTimer->start(5000);
}

void PhotoFrameBackend::onRtspFallbackTimeout() {
    stopRtsp();
    m_rtspErrorMsg = QString::fromUtf8("\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u0435 \u043a \u0441\u0435\u0440\u0432\u0435\u0440\u0443...");
    emit rtspErrorMsgChanged();
    emit rtspShowOverlay(m_rtspErrorMsg);
    QTimer::singleShot(500, this, [this]() {
        fallbackToPhotos();
    });
}

void PhotoFrameBackend::fallbackToPhotos() {
    if (m_config.server.isEmpty()) {
        setPageIndex(1);
        return;
    }
    connectAndScan();
}

void PhotoFrameBackend::onScanFinished(const QStringList& list) {
    m_scanning = false;
    qInfo() << "Scan finished: found" << list.size() << "files";
    if (list.isEmpty()) {
        qInfo() << "No files found, showing settings";
        setPageIndex(1);
        return;
    }
    m_playlist = list;
    PlaylistManager::save(m_playlist);
    if (m_config.shuffle)
        std::shuffle(m_playlist.begin(), m_playlist.end(), std::mt19937(std::random_device()()));
    m_idx = 0;

    if (m_rtspState != RtspPlaying) {
        m_rtspFallbackTimer->stop();
        m_rtspRetryTimer->stop();
        setRtspState(RtspIdle);
        emit rtspStopPlayer();
        emit rtspHideOverlay();
    }

    if (!m_isSleeping) {
        qInfo() << "Starting slideshow: interval=" << m_config.interval << "ms";
        m_slideshowTimer->start(m_config.interval);
        nextSlide();
    } else {
        qInfo() << "Sleeping, not starting slideshow";
    }
}

QStringList PhotoFrameBackend::tasks() const {
    QStringList result;
    QFile file("tasks.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (!line.isEmpty()) result << line;
        }
    }
    return result;
}

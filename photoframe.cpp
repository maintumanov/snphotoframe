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
    if (size) *size = m_current.size();
    if (requestedSize.isValid())
        return m_current.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    return m_current;
}

void ImageProvider::setCurrentImage(const QImage& img) {
    m_current = img;
}

// --- PhotoFrameBackend ---

PhotoFrameBackend::PhotoFrameBackend(QObject* parent)
    : QObject(parent)
{
    m_config.load();

    m_tickTimer = new QTimer(this);
    connect(m_tickTimer, &QTimer::timeout, this, &PhotoFrameBackend::onTick);
    m_tickTimer->start(1000);

    m_slideshowTimer = new QTimer(this);
    connect(m_slideshowTimer, &QTimer::timeout, this, &PhotoFrameBackend::nextSlide);

    if (m_config.useRtsp && !m_config.rtspUrl.isEmpty()) {
        emit rtspStarted(m_config.rtspUrl);
    } else {
        m_playlist = PlaylistManager::load();
        if (!m_playlist.isEmpty()) {
            if (m_config.shuffle)
                std::shuffle(m_playlist.begin(), m_playlist.end(), std::mt19937(std::random_device()()));
            checkSchedule();
            if (!m_isSleeping) {
                m_slideshowTimer->start(m_config.interval);
                QTimer::singleShot(500, this, &PhotoFrameBackend::nextSlide);
            }
        } else {
            QTimer::singleShot(1000, this, &PhotoFrameBackend::connectAndScan);
        }
    }
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
bool PhotoFrameBackend::useRtsp() const { return m_config.useRtsp; }
QString PhotoFrameBackend::rtspUrl() const { return m_config.rtspUrl; }

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
    if (m_playlist.isEmpty() || m_isSleeping) return;
    QString path = m_playlist[m_idx];
    m_idx = (m_idx + 1) % m_playlist.size();

    QtConcurrent::run([this, path]() {
        QImageReader reader(path);
        reader.setAutoTransform(true);
        QImage img = reader.read();
        if (m_destroyed) return;
        if (img.isNull()) {
            QMetaObject::invokeMethod(this, "nextSlide");
            return;
        }
        m_imageProvider->setCurrentImage(img);
        m_currentImagePath = "image://current/img";
        QMetaObject::invokeMethod(this, [this]() {
            emit imageChanged();
        });
    });
}

void PhotoFrameBackend::prevSlide() {
    if (m_isSleeping || m_playlist.isEmpty()) return;
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
    m_config.save();
    PlaylistManager::clear();

    if (m_config.useRtsp && !m_config.rtspUrl.isEmpty()) {
        m_slideshowTimer->stop();
        emit rtspStarted(m_config.rtspUrl);
    } else {
        emit rtspStopped();
        connectAndScan();
    }
    setPageIndex(0);
}

void PhotoFrameBackend::connectAndScan() {
    if (m_config.server.isEmpty()) {
        setPageIndex(1);
        return;
    }
    QtConcurrent::run([this]() {
#ifdef Q_OS_WIN
        QString path = QString(R"(\\%1\%2)").arg(m_config.server, m_config.share);
        QProcess::execute("net", {"use", path, m_config.pass, QString("/user:%1").arg(m_config.user)});
#else
        QDir().mkpath("/mnt/photoframe");
        QString opts = QString("vers=%1,username=%2,password=%3").arg(m_config.smbVers, m_config.user, m_config.pass);
        QProcess::execute("mount", {"-t", "cifs", QString("//%1/%2").arg(m_config.server, m_config.share), "/mnt/photoframe", "-o", opts});
#endif
        if (m_destroyed) return;
        QString scanPath =
#ifdef Q_OS_WIN
            QString(R"(\\%1\%2)").arg(m_config.server, m_config.share);
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

void PhotoFrameBackend::onScanFinished(const QStringList& list) {
    if (list.isEmpty()) { setPageIndex(1); return; }
    m_playlist = list;
    PlaylistManager::save(m_playlist);
    if (m_config.shuffle)
        std::shuffle(m_playlist.begin(), m_playlist.end(), std::mt19937(std::random_device()()));
    m_idx = 0;
    if (!m_isSleeping) {
        m_slideshowTimer->start(m_config.interval);
        nextSlide();
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

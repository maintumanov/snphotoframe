#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include "photoframe.h"
#include "signalnet.h"
#include "playlistmanager.h"

#include <QDateTime>
#include <QDirIterator>
#include <QImageReader>
#include <QFileInfo>
#include <QtConcurrent>
#include <QProcess>
#include <QCoreApplication>
#include <QQmlEngine>
#include <QSoundEffect>
#include <QUrl>
#include <QDir>
#include <QFile>
#include <cmath>
#include <algorithm>
#include <random>
#include <chrono>

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
    {
        QString snPath = QSNHomePath("photoframe").absoluteFilePath("photoframe.ini");
        QString oldPath = "photoframe.ini";
        if (QFile::exists(snPath)) {
            m_config.load(snPath);
        } else if (QFile::exists(oldPath)) {
            m_config.load(oldPath);
            m_config.save(snPath);
        } else {
            m_config.load(snPath);
        }
    }
    qInfo() << "Config loaded: server=" << m_config.server << "share=" << m_config.share
             << "useRtsp=" << m_config.useRtsp << "rtspUrl=" << m_config.rtspUrl
             << "useGuest=" << m_config.useGuest;

    initBacklight();

    // SignalNet
    m_signalNet = new SignalNet(this);
    connect(m_signalNet, &SignalNet::connectedChanged, this, &PhotoFrameBackend::signalNetConnectedChanged);
    connect(m_signalNet, &SignalNet::temperatureChanged, this, &PhotoFrameBackend::signalNetTemperatureChanged);
    connect(m_signalNet, &SignalNet::lastAlertChanged, this, &PhotoFrameBackend::signalNetAlertChanged);
    connect(m_signalNet, &SignalNet::alertSeverityChanged, this, &PhotoFrameBackend::signalNetAlertSeverityChanged);
    connect(m_signalNet, &SignalNet::temperatureValidChanged, this, &PhotoFrameBackend::signalNetTemperatureValidChanged);
    connect(m_signalNet, &SignalNet::temperatureOutChanged, this, &PhotoFrameBackend::signalNetTemperatureOutChanged);
    connect(m_signalNet, &SignalNet::temperatureOutValidChanged, this, &PhotoFrameBackend::signalNetTemperatureOutValidChanged);
    connect(m_signalNet, &SignalNet::humidityChanged, this, &PhotoFrameBackend::signalNetHumidityChanged);
    connect(m_signalNet, &SignalNet::humidityValidChanged, this, &PhotoFrameBackend::signalNetHumidityValidChanged);
    connect(m_signalNet, &SignalNet::co2Changed, this, &PhotoFrameBackend::signalNetCo2Changed);
    connect(m_signalNet, &SignalNet::co2ValidChanged, this, &PhotoFrameBackend::signalNetCo2ValidChanged);
    connect(m_signalNet, &SignalNet::dustChanged, this, &PhotoFrameBackend::signalNetDustChanged);
    connect(m_signalNet, &SignalNet::dustValidChanged, this, &PhotoFrameBackend::signalNetDustValidChanged);
    connect(m_signalNet, &SignalNet::varChanged, this, &PhotoFrameBackend::signalNetVarChanged);
    connect(m_signalNet, &SignalNet::varValidChanged, this, &PhotoFrameBackend::signalNetVarValidChanged);
    connect(m_signalNet, &SignalNet::mediaNext, this, &PhotoFrameBackend::nextSlide);
    connect(m_signalNet, &SignalNet::mediaPrevious, this, &PhotoFrameBackend::prevSlide);
    connect(m_signalNet, &SignalNet::mediaPlayPause, this, &PhotoFrameBackend::toggleSlideshow);
    // bellPressed handled inside SignalNet — sets lastAlert directly
    connect(m_signalNet, &SignalNet::absenceModeChanged, this, [this]() {
        bool absent = m_signalNet->absenceMode();
        qInfo() << "Absence mode" << (absent ? "ON" : "OFF");
        if (absent) {
            stopRtsp();
            stopRtsp2();
            stopRtsp3();
        }
        setSleepMode(absent);
    });
    if (m_config.useSignalNet && !m_config.signalNetServer.isEmpty()) {
        QTimer::singleShot(2000, this, &PhotoFrameBackend::connectSignalNet);
    }

    // Alert sound — generate sine wave WAV in temp file
    m_alertSound = new QSoundEffect(this);
    m_alertSound->setLoopCount(3);
    m_alertSound->setVolume(0.8f);
    {
        const int sampleRate = 44100;
        const int durationMs = 200;
        const int freq = 880;
        const int samples = sampleRate * durationMs / 1000;
        QByteArray wav;
        // WAV header
        wav.append("RIFF", 4);
        int dataSize = samples * 2;
        int fileSize = 36 + dataSize;
        wav.append(reinterpret_cast<const char*>(&fileSize), 4);
        wav.append("WAVE", 4);
        wav.append("fmt ", 4);
        int fmtSize = 16;
        wav.append(reinterpret_cast<const char*>(&fmtSize), 4);
        short fmt = 1; wav.append(reinterpret_cast<const char*>(&fmt), 2);
        short ch = 1; wav.append(reinterpret_cast<const char*>(&ch), 2);
        int sr = sampleRate; wav.append(reinterpret_cast<const char*>(&sr), 4);
        int byteRate = sampleRate * 2; wav.append(reinterpret_cast<const char*>(&byteRate), 4);
        short blockAlign = 2; wav.append(reinterpret_cast<const char*>(&blockAlign), 2);
        short bits = 16; wav.append(reinterpret_cast<const char*>(&bits), 2);
        wav.append("data", 4);
        wav.append(reinterpret_cast<const char*>(&dataSize), 4);
        // Sine wave samples
        for (int i = 0; i < samples; i++) {
            double t = (double)i / sampleRate;
            short sample = (short)(16000.0 * sin(2.0 * M_PI * freq * t));
            wav.append(reinterpret_cast<const char*>(&sample), 2);
        }
        QString tmpPath = QDir::tempPath() + "/photoframe_alert.wav";
        QFile tmpFile(tmpPath);
        if (tmpFile.open(QIODevice::WriteOnly)) {
            tmpFile.write(wav);
            tmpFile.close();
            m_alertSound->setSource(QUrl::fromLocalFile(tmpPath));
        }
    }
    connect(m_signalNet, &SignalNet::alertReceived, this, [this](const QString &, int) {
        if (m_alertSound && m_alertSound->status() != QSoundEffect::Error)
            m_alertSound->play();
    });

    // Camera on from SignalNet — start RTSP for configured duration
    m_cameraTimer = new QTimer(this);
    m_cameraTimer->setSingleShot(true);
    connect(m_cameraTimer, &QTimer::timeout, this, &PhotoFrameBackend::onCameraTimeout);
    connect(m_signalNet, &SignalNet::cameraOn, this, [this]() {
        if (m_config.useRtsp && !m_config.rtspUrl.isEmpty() && m_rtspState == RtspIdle) {
            qInfo() << "Camera ON via SignalNet for" << m_config.cameraDuration << "sec";
            startRtsp();
            m_cameraTimer->start(m_config.cameraDuration * 1000);
        }
    });

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

    // Camera 2 on from SignalNet
    m_camera2Timer = new QTimer(this);
    m_camera2Timer->setSingleShot(true);
    connect(m_camera2Timer, &QTimer::timeout, this, &PhotoFrameBackend::onCamera2Timeout);
    connect(m_signalNet, &SignalNet::camera2On, this, [this]() {
        if (m_config.useRtsp2 && !m_config.rtspUrl2.isEmpty() && m_rtsp2State == RtspIdle) {
            qInfo() << "Camera2 ON via SignalNet for" << m_config.camera2Duration << "sec";
            startRtsp2();
            m_camera2Timer->start(m_config.camera2Duration * 1000);
        }
    });

    // Camera 2 retry/fallback timers
    m_rtsp2RetryTimer = new QTimer(this);
    m_rtsp2RetryTimer->setSingleShot(true);
    connect(m_rtsp2RetryTimer, &QTimer::timeout, this, &PhotoFrameBackend::onRtsp2RetryTimeout);
    m_rtsp2FallbackTimer = new QTimer(this);
    m_rtsp2FallbackTimer->setSingleShot(true);
    connect(m_rtsp2FallbackTimer, &QTimer::timeout, this, &PhotoFrameBackend::onRtsp2FallbackTimeout);

    // Camera 3 on from SignalNet
    m_camera3Timer = new QTimer(this);
    m_camera3Timer->setSingleShot(true);
    connect(m_camera3Timer, &QTimer::timeout, this, &PhotoFrameBackend::onCamera3Timeout);
    connect(m_signalNet, &SignalNet::camera3On, this, [this]() {
        if (m_config.useRtsp3 && !m_config.rtspUrl3.isEmpty() && m_rtsp3State == RtspIdle) {
            qInfo() << "Camera3 ON via SignalNet for" << m_config.camera3Duration << "sec";
            startRtsp3();
            m_camera3Timer->start(m_config.camera3Duration * 1000);
        }
    });

    // Camera 3 retry/fallback timers
    m_rtsp3RetryTimer = new QTimer(this);
    m_rtsp3RetryTimer->setSingleShot(true);
    connect(m_rtsp3RetryTimer, &QTimer::timeout, this, &PhotoFrameBackend::onRtsp3RetryTimeout);
    m_rtsp3FallbackTimer = new QTimer(this);
    m_rtsp3FallbackTimer->setSingleShot(true);
    connect(m_rtsp3FallbackTimer, &QTimer::timeout, this, &PhotoFrameBackend::onRtsp3FallbackTimeout);

    // Defer all startup to after QML loads — signals need listeners
    QTimer::singleShot(500, this, [this]() {
        m_playlist = PlaylistManager::load();
        if (!m_playlist.isEmpty()) {
            qInfo() << "Loaded" << m_playlist.size() << "cached files";
            if (m_config.shuffle) {
                auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
                std::shuffle(m_playlist.begin(), m_playlist.end(), std::mt19937(static_cast<unsigned>(seed)));
            }
            checkSchedule();
            if (!m_isSleeping) {
                m_slideshowTimer->start(m_config.interval);
                nextSlide();
            }
        }
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
    return QDate::currentDate().toString("dd.MM.yyyy");
}

QString PhotoFrameBackend::currentImagePath() const {
    return m_currentImagePath;
}

QString PhotoFrameBackend::currentFileName() const {
    return m_currentFileName;
}

QString PhotoFrameBackend::currentFileDate() const {
    return m_currentFileDate;
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
bool PhotoFrameBackend::useActionButtons() const { return m_config.useActionButtons; }
void PhotoFrameBackend::setUseActionButtons(bool v) { if (m_config.useActionButtons != v) { m_config.useActionButtons = v; emit configChanged(); } }
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

bool PhotoFrameBackend::useRtsp2() const { return m_config.useRtsp2; }
QString PhotoFrameBackend::rtspUrl2() const { return m_config.rtspUrl2; }
int PhotoFrameBackend::camera2Duration() const { return m_config.camera2Duration; }
void PhotoFrameBackend::setUseRtsp2(bool v) { if (m_config.useRtsp2 != v) { m_config.useRtsp2 = v; emit configChanged(); } }
void PhotoFrameBackend::setRtspUrl2(const QString& v) { if (m_config.rtspUrl2 != v) { m_config.rtspUrl2 = v; emit configChanged(); } }
void PhotoFrameBackend::setCamera2Duration(int v) { if (m_config.camera2Duration != v) { m_config.camera2Duration = v; emit configChanged(); } }

bool PhotoFrameBackend::useRtsp3() const { return m_config.useRtsp3; }
QString PhotoFrameBackend::rtspUrl3() const { return m_config.rtspUrl3; }
int PhotoFrameBackend::camera3Duration() const { return m_config.camera3Duration; }
void PhotoFrameBackend::setUseRtsp3(bool v) { if (m_config.useRtsp3 != v) { m_config.useRtsp3 = v; emit configChanged(); } }
void PhotoFrameBackend::setRtspUrl3(const QString& v) { if (m_config.rtspUrl3 != v) { m_config.rtspUrl3 = v; emit configChanged(); } }
void PhotoFrameBackend::setCamera3Duration(int v) { if (m_config.camera3Duration != v) { m_config.camera3Duration = v; emit configChanged(); } }

int PhotoFrameBackend::rtsp2State() const { return m_rtsp2State; }
QString PhotoFrameBackend::rtsp2ErrorMsg() const { return m_rtsp2ErrorMsg; }
int PhotoFrameBackend::rtsp3State() const { return m_rtsp3State; }
QString PhotoFrameBackend::rtsp3ErrorMsg() const { return m_rtsp3ErrorMsg; }

// SignalNet getters
bool PhotoFrameBackend::useSignalNet() const { return m_config.useSignalNet; }
QString PhotoFrameBackend::signalNetServer() const { return m_config.signalNetServer; }
int PhotoFrameBackend::signalNetPort() const { return m_config.signalNetPort; }
QString PhotoFrameBackend::signalNetLogin() const { return m_config.signalNetLogin; }
QString PhotoFrameBackend::signalNetPass() const { return m_config.signalNetPass; }
bool PhotoFrameBackend::signalNetConnected() const { return m_signalNet && m_signalNet->isConnected(); }
qreal PhotoFrameBackend::signalNetTemperature() const { return m_signalNet ? m_signalNet->temperature() : 0; }
QString PhotoFrameBackend::signalNetAlert() const { return m_signalNet ? m_signalNet->lastAlert() : QString(); }
int PhotoFrameBackend::signalNetAlertSeverity() const { return m_signalNet ? m_signalNet->alertSeverity() : 0; }
bool PhotoFrameBackend::signalNetTemperatureValid() const { return m_signalNet && m_signalNet->isTemperatureValid(); }
qreal PhotoFrameBackend::signalNetTemperatureOut() const { return m_signalNet ? m_signalNet->temperatureOut() : 0; }
bool PhotoFrameBackend::signalNetTemperatureOutValid() const { return m_signalNet && m_signalNet->isTemperatureOutValid(); }
qreal PhotoFrameBackend::signalNetHumidity() const { return m_signalNet ? m_signalNet->humidity() : 0; }
bool PhotoFrameBackend::signalNetHumidityValid() const { return m_signalNet && m_signalNet->isHumidityValid(); }
int PhotoFrameBackend::signalNetCo2() const { return m_signalNet ? m_signalNet->co2() : 0; }
bool PhotoFrameBackend::signalNetCo2Valid() const { return m_signalNet && m_signalNet->isCo2Valid(); }
int PhotoFrameBackend::signalNetDust() const { return m_signalNet ? m_signalNet->dust() : 0; }
bool PhotoFrameBackend::signalNetDustValid() const { return m_signalNet && m_signalNet->isDustValid(); }
qreal PhotoFrameBackend::signalNetVar() const { return m_signalNet ? m_signalNet->var() : 0; }
bool PhotoFrameBackend::signalNetVarValid() const { return m_signalNet && m_signalNet->isVarValid(); }
int PhotoFrameBackend::cameraDuration() const { return m_config.cameraDuration; }
void PhotoFrameBackend::setCameraDuration(int v) { if (m_config.cameraDuration != v) { m_config.cameraDuration = v; emit configChanged(); } }
int PhotoFrameBackend::signalNetDeviceAddress() const { return m_config.signalNetDeviceAddress; }
void PhotoFrameBackend::setSignalNetDeviceAddress(int v) {
    quint16 addr = static_cast<quint16>(v);
    if (m_config.signalNetDeviceAddress != addr) {
        m_config.signalNetDeviceAddress = addr;
        if (m_signalNet) m_signalNet->setDeviceAddress(addr);
        emit configChanged();
    }
}

int PhotoFrameBackend::brightness() const { return m_config.brightness; }

void PhotoFrameBackend::setBrightness(int v) {
    v = qBound(0, v, 100);
    if (m_config.brightness != v) {
        m_config.brightness = v;
        applyBacklight(v);
        emit configChanged();
    }
}

bool PhotoFrameBackend::backlightAvailable() const {
    return m_backlightAvailable;
}

void PhotoFrameBackend::initBacklight() {
#ifdef Q_OS_LINUX
    m_backlightAvailable = QFile::exists("/sys/class/backlight/10-0045/brightness");
    if (m_backlightAvailable) {
        QFile maxFile("/sys/class/backlight/10-0045/max_brightness");
        if (maxFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            bool ok = false;
            int max = maxFile.readAll().trimmed().toInt(&ok);
            maxFile.close();
            if (ok && max > 0) m_backlightMax = max;
        }
        applyBacklight(m_config.brightness);
    }
#else
    m_backlightAvailable = false;
#endif
}

void PhotoFrameBackend::applyBacklight(int pct) {
    if (!m_backlightAvailable) return;
    int value = qBound(0, (pct * m_backlightMax + 50) / 100, m_backlightMax);
    QFile file("/sys/class/backlight/10-0045/brightness");
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(QByteArray::number(value));
        file.close();
    }
}


void PhotoFrameBackend::onCameraTimeout() {
    qInfo() << "Camera OFF — timeout";
    stopRtsp();
}

// --- Camera 2 ---
void PhotoFrameBackend::startRtsp2() {
    if (!m_config.useRtsp2 || m_config.rtspUrl2.isEmpty()) return;
    m_slideshowTimer->stop();
    m_rtsp2RetryCount = 0;
    m_rtsp2RetryTimer->stop();
    m_rtsp2FallbackTimer->stop();
    setRtsp2State(RtspConnecting);
    m_rtsp2ErrorMsg = QString::fromUtf8("\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u0435...");
    emit rtsp2ErrorMsgChanged();
    emit rtsp2ShowOverlay(m_rtsp2ErrorMsg);
    m_rtsp2FallbackTimer->start(5000);
    emit rtsp2Play(m_config.rtspUrl2);
}

void PhotoFrameBackend::stopRtsp2() {
    m_rtsp2RetryTimer->stop();
    m_rtsp2FallbackTimer->stop();
    m_rtsp2RetryCount = 0;
    setRtsp2State(RtspIdle);
    emit rtsp2StopPlayer();
    emit rtsp2HideOverlay();
    resumeSlideshow();
}

void PhotoFrameBackend::reconnectRtsp2() {
    stopRtsp2();
    startRtsp2();
}

void PhotoFrameBackend::setRtsp2State(RtspState s) {
    if (m_rtsp2State != s) {
        m_rtsp2State = s;
        emit rtsp2StateChanged();
    }
}

void PhotoFrameBackend::onRtsp2Playing() {
    m_rtsp2FallbackTimer->stop();
    m_rtsp2RetryTimer->stop();
    m_rtsp2RetryCount = 0;
    setRtsp2State(RtspPlaying);
    QTimer::singleShot(5000, this, [this]() {
        if (m_rtsp2State == RtspPlaying)
            emit rtsp2HideOverlay();
    });
}

void PhotoFrameBackend::onRtsp2Error(const QString& msg) {
    if (m_rtsp2State == RtspIdle) return;
    m_rtsp2FallbackTimer->stop();
    m_rtsp2RetryCount++;
    if (m_rtsp2RetryCount <= kMaxRtspRetries) {
        m_rtsp2ErrorMsg = msg + QString::fromUtf8(" \u2014 \u043f\u043e\u0432\u0442\u043e\u0440 %1/%2").arg(m_rtsp2RetryCount).arg(kMaxRtspRetries);
        emit rtsp2ErrorMsgChanged();
        emit rtsp2ShowOverlay(m_rtsp2ErrorMsg);
        setRtsp2State(RtspError);
        m_rtsp2RetryTimer->start(2000);
    } else {
        onRtsp2FallbackTimeout();
    }
}

void PhotoFrameBackend::onRtsp2RetryTimeout() {
    if (m_rtsp2State == RtspIdle) return;
    m_rtsp2ErrorMsg = QString::fromUtf8("\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u0435... \u043f\u043e\u0432\u0442\u043e\u0440 %1/%2").arg(m_rtsp2RetryCount + 1).arg(kMaxRtspRetries);
    emit rtsp2ErrorMsgChanged();
    emit rtsp2ShowOverlay(m_rtsp2ErrorMsg);
    setRtsp2State(RtspConnecting);
    emit rtsp2Play(m_config.rtspUrl2);
    m_rtsp2FallbackTimer->start(5000);
}

void PhotoFrameBackend::onRtsp2FallbackTimeout() {
    stopRtsp2();
    m_rtsp2ErrorMsg = QString::fromUtf8("\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u0435 \u043a \u0441\u0435\u0440\u0432\u0435\u0440\u0443...");
    emit rtsp2ErrorMsgChanged();
    emit rtsp2ShowOverlay(m_rtsp2ErrorMsg);
    QTimer::singleShot(500, this, [this]() { fallbackToPhotos(); });
}

void PhotoFrameBackend::onCamera2Timeout() {
    qInfo() << "Camera2 OFF — timeout";
    stopRtsp2();
}

// --- Camera 3 ---
void PhotoFrameBackend::startRtsp3() {
    if (!m_config.useRtsp3 || m_config.rtspUrl3.isEmpty()) return;
    m_slideshowTimer->stop();
    m_rtsp3RetryCount = 0;
    m_rtsp3RetryTimer->stop();
    m_rtsp3FallbackTimer->stop();
    setRtsp3State(RtspConnecting);
    m_rtsp3ErrorMsg = QString::fromUtf8("\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u0435...");
    emit rtsp3ErrorMsgChanged();
    emit rtsp3ShowOverlay(m_rtsp3ErrorMsg);
    m_rtsp3FallbackTimer->start(5000);
    emit rtsp3Play(m_config.rtspUrl3);
}

void PhotoFrameBackend::stopRtsp3() {
    m_rtsp3RetryTimer->stop();
    m_rtsp3FallbackTimer->stop();
    m_rtsp3RetryCount = 0;
    setRtsp3State(RtspIdle);
    emit rtsp3StopPlayer();
    emit rtsp3HideOverlay();
    resumeSlideshow();
}

void PhotoFrameBackend::reconnectRtsp3() {
    stopRtsp3();
    startRtsp3();
}

void PhotoFrameBackend::setRtsp3State(RtspState s) {
    if (m_rtsp3State != s) {
        m_rtsp3State = s;
        emit rtsp3StateChanged();
    }
}

void PhotoFrameBackend::onRtsp3Playing() {
    m_rtsp3FallbackTimer->stop();
    m_rtsp3RetryTimer->stop();
    m_rtsp3RetryCount = 0;
    setRtsp3State(RtspPlaying);
    QTimer::singleShot(5000, this, [this]() {
        if (m_rtsp3State == RtspPlaying)
            emit rtsp3HideOverlay();
    });
}

void PhotoFrameBackend::onRtsp3Error(const QString& msg) {
    if (m_rtsp3State == RtspIdle) return;
    m_rtsp3FallbackTimer->stop();
    m_rtsp3RetryCount++;
    if (m_rtsp3RetryCount <= kMaxRtspRetries) {
        m_rtsp3ErrorMsg = msg + QString::fromUtf8(" \u2014 \u043f\u043e\u0432\u0442\u043e\u0440 %1/%2").arg(m_rtsp3RetryCount).arg(kMaxRtspRetries);
        emit rtsp3ErrorMsgChanged();
        emit rtsp3ShowOverlay(m_rtsp3ErrorMsg);
        setRtsp3State(RtspError);
        m_rtsp3RetryTimer->start(2000);
    } else {
        onRtsp3FallbackTimeout();
    }
}

void PhotoFrameBackend::onRtsp3RetryTimeout() {
    if (m_rtsp3State == RtspIdle) return;
    m_rtsp3ErrorMsg = QString::fromUtf8("\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u0435... \u043f\u043e\u0432\u0442\u043e\u0440 %1/%2").arg(m_rtsp3RetryCount + 1).arg(kMaxRtspRetries);
    emit rtsp3ErrorMsgChanged();
    emit rtsp3ShowOverlay(m_rtsp3ErrorMsg);
    setRtsp3State(RtspConnecting);
    emit rtsp3Play(m_config.rtspUrl3);
    m_rtsp3FallbackTimer->start(5000);
}

void PhotoFrameBackend::onRtsp3FallbackTimeout() {
    stopRtsp3();
    m_rtsp3ErrorMsg = QString::fromUtf8("\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u0435 \u043a \u0441\u0435\u0440\u0432\u0435\u0440\u0443...");
    emit rtsp3ErrorMsgChanged();
    emit rtsp3ShowOverlay(m_rtsp3ErrorMsg);
    QTimer::singleShot(500, this, [this]() { fallbackToPhotos(); });
}

void PhotoFrameBackend::onCamera3Timeout() {
    qInfo() << "Camera3 OFF — timeout";
    stopRtsp3();
}

// SignalNet setters
void PhotoFrameBackend::setUseSignalNet(bool v) { if (m_config.useSignalNet != v) { m_config.useSignalNet = v; emit configChanged(); } }
void PhotoFrameBackend::setSignalNetServer(const QString& v) { if (m_config.signalNetServer != v) { m_config.signalNetServer = v; emit configChanged(); } }
void PhotoFrameBackend::setSignalNetPort(int v) { if (m_config.signalNetPort != static_cast<quint16>(v)) { m_config.signalNetPort = static_cast<quint16>(v); emit configChanged(); } }
void PhotoFrameBackend::setSignalNetLogin(const QString& v) { if (m_config.signalNetLogin != v) { m_config.signalNetLogin = v; emit configChanged(); } }
void PhotoFrameBackend::setSignalNetPass(const QString& v) { if (m_config.signalNetPass != v) { m_config.signalNetPass = v; emit configChanged(); } }

bool PhotoFrameBackend::signalNetUseUdp() const { return m_config.useUdp; }
void PhotoFrameBackend::setSignalNetUseUdp(bool v) { if (m_config.useUdp != v) { m_config.useUdp = v; emit configChanged(); } }
int PhotoFrameBackend::signalNetUdpLocalPort() const { return m_config.signalNetUdpLocalPort; }
void PhotoFrameBackend::setSignalNetUdpLocalPort(int v) { if (m_config.signalNetUdpLocalPort != static_cast<quint16>(v)) { m_config.signalNetUdpLocalPort = static_cast<quint16>(v); emit configChanged(); } }
QString PhotoFrameBackend::signalNetUdpKey() const { return m_config.signalNetUdpKey; }
void PhotoFrameBackend::setSignalNetUdpKey(const QString& v) { if (m_config.signalNetUdpKey != v) { m_config.signalNetUdpKey = v; emit configChanged(); } }

void PhotoFrameBackend::connectSignalNet() {
    if (!m_signalNet) return;
    if (m_config.useUdp) {
        m_signalNet->connectToServerUDP(m_config.signalNetServer, m_config.signalNetPort,
                                        m_config.signalNetUdpLocalPort, m_config.signalNetUdpKey);
    } else {
        m_signalNet->connectToServer(m_config.signalNetServer, m_config.signalNetPort,
                                     m_config.signalNetLogin, m_config.signalNetPass);
    }
}

void PhotoFrameBackend::disconnectSignalNet() {
    if (m_signalNet) m_signalNet->disconnectFromServer();
}

void PhotoFrameBackend::clearSignalNetAlert() {
    if (m_signalNet) m_signalNet->clearAlert();
}

void PhotoFrameBackend::sendAction1() {
    if (m_signalNet) m_signalNet->sendAction1();
}

void PhotoFrameBackend::sendAction2() {
    if (m_signalNet) m_signalNet->sendAction2();
}

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
        applyBacklight(kSleepBacklight);
    } else {
        applyBacklight(m_config.brightness);
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
        m_currentFileName = QFileInfo(loadedPath).fileName();
        // Extract photo date from EXIF or file modification time
        QImageReader dateReader(loadedPath);
        QString exifDate = dateReader.text("DateTimeOriginal");
        if (exifDate.isEmpty()) exifDate = dateReader.text("DateTime");
        if (!exifDate.isEmpty()) {
            // EXIF format: "2024:01:15 14:30:00"
            QDateTime dt = QDateTime::fromString(exifDate, "yyyy:MM:dd HH:mm:ss");
            m_currentFileDate = dt.isValid() ? dt.toString("dd.MM.yyyy HH:mm") : exifDate;
        } else {
            m_currentFileDate = QFileInfo(loadedPath).lastModified().toString("dd.MM.yyyy HH:mm");
        }
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

void PhotoFrameBackend::firstSlide() {
    if (m_isSleeping || m_playlist.isEmpty()) return;
    // Go to the very first image: set idx so nextSlide() loads index 0
    m_idx = m_playlist.size() - 1;
    nextSlide();
}

void PhotoFrameBackend::lastSlide() {
    if (m_isSleeping || m_playlist.isEmpty()) return;
    // Go to the very last image: set idx so nextSlide() loads index size-1
    m_idx = m_playlist.size() - 2;
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
    m_config.save(QSNHomePath("photoframe").absoluteFilePath("photoframe.ini"));
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
        QProcess::execute("sudo", {"mount", "-t", "cifs", QString("//%1/%2").arg(server, share), "/mnt/photoframe", "-o", opts});
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

void PhotoFrameBackend::resumeSlideshow() {
    if (m_isSleeping || m_playlist.isEmpty()) {
        qInfo() << "resumeSlideshow skipped: sleeping=" << m_isSleeping << "playlist=" << m_playlist.size();
        return;
    }
    if (m_rtspState != RtspIdle || m_rtsp2State != RtspIdle || m_rtsp3State != RtspIdle)
        return; // an RTSP session is still active
    m_slideshowTimer->stop();
    m_slideshowTimer->start(m_config.interval);
    nextSlide();
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
    resumeSlideshow();
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
    if (m_config.shuffle) {
        auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
        std::shuffle(m_playlist.begin(), m_playlist.end(), std::mt19937(static_cast<unsigned>(seed)));
    }
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

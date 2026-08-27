#ifndef PHOTOFRAME_H
#define PHOTOFRAME_H

#include <QObject>
#include <QTimer>
#include <QStringList>
#include <QImage>
#include <QQuickImageProvider>
#include <QTime>
#include <QDate>
#include <QUrl>
#include <QMutex>
#include <QMutexLocker>
#include <QSoundEffect>
#include "config.h"
class SignalNet;

class ImageProvider : public QQuickImageProvider {
public:
    ImageProvider();
    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;
    void setCurrentImage(const QImage& img);
private:
    mutable QMutex m_mutex;
    QImage m_current;
};

class PhotoFrameBackend : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString currentTime READ currentTime NOTIFY tick)
    Q_PROPERTY(QString currentDate READ currentDate NOTIFY tick)
    Q_PROPERTY(QString currentImagePath READ currentImagePath NOTIFY imageChanged)
    Q_PROPERTY(int pageIndex READ pageIndex WRITE setPageIndex NOTIFY pageIndexChanged)

    Q_PROPERTY(QString server READ server WRITE setServer NOTIFY configChanged)
    Q_PROPERTY(QString share READ share WRITE setShare NOTIFY configChanged)
    Q_PROPERTY(QString user READ user WRITE setUser NOTIFY configChanged)
    Q_PROPERTY(QString pass READ pass WRITE setPass NOTIFY configChanged)
    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY configChanged)
    Q_PROPERTY(bool shuffle READ shuffle WRITE setShuffle NOTIFY configChanged)

    Q_PROPERTY(bool useSchedule READ useSchedule WRITE setUseSchedule NOTIFY configChanged)
    Q_PROPERTY(QString wakeTime READ wakeTimeStr WRITE setWakeTimeStr NOTIFY configChanged)
    Q_PROPERTY(QString sleepTime READ sleepTimeStr WRITE setSleepTimeStr NOTIFY configChanged)

    Q_PROPERTY(bool useGuest READ useGuest WRITE setUseGuest NOTIFY configChanged)
    Q_PROPERTY(QString smbVers READ smbVers WRITE setSmbVers NOTIFY configChanged)
    Q_PROPERTY(bool useRtsp READ useRtsp WRITE setUseRtsp NOTIFY configChanged)
    Q_PROPERTY(QString rtspUrl READ rtspUrl WRITE setRtspUrl NOTIFY configChanged)

    Q_PROPERTY(int rtspState READ rtspState NOTIFY rtspStateChanged)
    Q_PROPERTY(QString rtspErrorMsg READ rtspErrorMsg NOTIFY rtspErrorMsgChanged)

    // SignalNet
    Q_PROPERTY(bool useSignalNet READ useSignalNet WRITE setUseSignalNet NOTIFY configChanged)
    Q_PROPERTY(QString signalNetServer READ signalNetServer WRITE setSignalNetServer NOTIFY configChanged)
    Q_PROPERTY(int signalNetPort READ signalNetPort WRITE setSignalNetPort NOTIFY configChanged)
    Q_PROPERTY(QString signalNetLogin READ signalNetLogin WRITE setSignalNetLogin NOTIFY configChanged)
    Q_PROPERTY(QString signalNetPass READ signalNetPass WRITE setSignalNetPass NOTIFY configChanged)
    Q_PROPERTY(bool signalNetConnected READ signalNetConnected NOTIFY signalNetConnectedChanged)
    Q_PROPERTY(qreal signalNetTemperature READ signalNetTemperature NOTIFY signalNetTemperatureChanged)
    Q_PROPERTY(QString signalNetAlert READ signalNetAlert NOTIFY signalNetAlertChanged)
    Q_PROPERTY(int signalNetAlertSeverity READ signalNetAlertSeverity NOTIFY signalNetAlertSeverityChanged)
    Q_PROPERTY(bool signalNetTemperatureValid READ signalNetTemperatureValid NOTIFY signalNetTemperatureValidChanged)
    Q_PROPERTY(bool signalNetUseUdp READ signalNetUseUdp WRITE setSignalNetUseUdp NOTIFY configChanged)
    Q_PROPERTY(int signalNetUdpLocalPort READ signalNetUdpLocalPort WRITE setSignalNetUdpLocalPort NOTIFY configChanged)
    Q_PROPERTY(QString signalNetUdpKey READ signalNetUdpKey WRITE setSignalNetUdpKey NOTIFY configChanged)

public:
    explicit PhotoFrameBackend(QObject* parent = nullptr);
    ~PhotoFrameBackend();
    void setImageProvider(ImageProvider* p) { m_imageProvider = p; }

    QString currentTime() const;
    QString currentDate() const;
    QString currentImagePath() const;
    int pageIndex() const;
    void setPageIndex(int p);

    QString server() const;
    void setServer(const QString& v);
    QString share() const;
    void setShare(const QString& v);
    QString user() const;
    void setUser(const QString& v);
    QString pass() const;
    void setPass(const QString& v);
    int interval() const;
    void setInterval(int v);
    bool shuffle() const;
    void setShuffle(bool v);

    bool useSchedule() const;
    void setUseSchedule(bool v);
    QString wakeTimeStr() const;
    void setWakeTimeStr(const QString& v);
    QString sleepTimeStr() const;
    void setSleepTimeStr(const QString& v);

    bool useGuest() const;
    void setUseGuest(bool v);
    QString smbVers() const;
    void setSmbVers(const QString& v);
    bool useRtsp() const;
    void setUseRtsp(bool v);
    QString rtspUrl() const;
    void setRtspUrl(const QString& v);

    Q_INVOKABLE void nextSlide();
    Q_INVOKABLE void prevSlide();
    Q_INVOKABLE void toggleSlideshow();
    Q_INVOKABLE void saveSettings();
    Q_INVOKABLE void connectAndScan();
    Q_INVOKABLE void reconnectRtsp();
    Q_INVOKABLE void fallbackToPhotos();
    Q_INVOKABLE QStringList tasks() const;

    // Called by QML MediaPlayer to report state back to backend
    Q_INVOKABLE void onRtspPlaying();
    Q_INVOKABLE void onRtspError(const QString& msg);
    Q_INVOKABLE void stopRtsp();

    // SignalNet
    bool useSignalNet() const;
    void setUseSignalNet(bool v);
    QString signalNetServer() const;
    void setSignalNetServer(const QString& v);
    int signalNetPort() const;
    void setSignalNetPort(int v);
    QString signalNetLogin() const;
    void setSignalNetLogin(const QString& v);
    QString signalNetPass() const;
    void setSignalNetPass(const QString& v);
    bool signalNetUseUdp() const;
    void setSignalNetUseUdp(bool v);
    int signalNetUdpLocalPort() const;
    void setSignalNetUdpLocalPort(int v);
    QString signalNetUdpKey() const;
    void setSignalNetUdpKey(const QString& v);
    bool signalNetConnected() const;
    qreal signalNetTemperature() const;
    QString signalNetAlert() const;
    int signalNetAlertSeverity() const;
    bool signalNetTemperatureValid() const;

    Q_INVOKABLE void connectSignalNet();
    Q_INVOKABLE void disconnectSignalNet();
    Q_INVOKABLE void clearSignalNetAlert();

enum RtspState { RtspIdle = 0, RtspConnecting, RtspPlaying, RtspError };
    Q_ENUM(RtspState)

    int rtspState() const;
    QString rtspErrorMsg() const;

signals:
    void tick();
    void imageChanged();
    void pageIndexChanged();
    void configChanged();
    void sleepChanged(bool sleeping);
    void showSettingsPage();
    void rtspStarted(const QString& url);

    // RTSP lifecycle signals — QML binds MediaPlayer/overlay to these
    void rtspPlay(const QString& url);
    void rtspStopPlayer();
    void rtspShowOverlay(const QString& msg);
    void rtspHideOverlay();
    void rtspStateChanged();
    void rtspErrorMsgChanged();

    // SignalNet signals
    void signalNetConnectedChanged();
    void signalNetTemperatureChanged();
    void signalNetAlertChanged();
    void signalNetAlertSeverityChanged();
    void signalNetTemperatureValidChanged();

private slots:
    void onTick();
    void onScanFinished(const QStringList& list);
    void onRtspRetryTimeout();
    void onRtspFallbackTimeout();

private:
    void checkSchedule();
    void setSleepMode(bool sleep);

    void startRtsp();
    void setRtspState(RtspState s);

    QTimer* m_tickTimer;
    QTimer* m_slideshowTimer;
    QTimer* m_rtspRetryTimer;
    QTimer* m_rtspFallbackTimer;
    SmbConfig m_config;
    QStringList m_playlist;
    int m_idx = 0;
    int m_pageIndex = 0;
    bool m_isSleeping = false;
    bool m_destroyed = false;
    bool m_scanning = false;
    QString m_currentImagePath;
    int m_imageCounter = 0;
    static const int kMaxConsecutiveFails = 50;
    RtspState m_rtspState = RtspIdle;
    int m_rtspRetryCount = 0;
    static const int kMaxRtspRetries = 3;
    QString m_rtspErrorMsg;
    ImageProvider* m_imageProvider = nullptr;
    SignalNet* m_signalNet = nullptr;
    QSoundEffect* m_alertSound = nullptr;

};

#endif // PHOTOFRAME_H

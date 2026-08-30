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
    Q_PROPERTY(QString currentFileName READ currentFileName NOTIFY imageChanged)
    Q_PROPERTY(QString currentFileDate READ currentFileDate NOTIFY imageChanged)
    Q_PROPERTY(int pageIndex READ pageIndex WRITE setPageIndex NOTIFY pageIndexChanged)

    Q_PROPERTY(QString server READ server WRITE setServer NOTIFY configChanged)
    Q_PROPERTY(QString share READ share WRITE setShare NOTIFY configChanged)
    Q_PROPERTY(QString user READ user WRITE setUser NOTIFY configChanged)
    Q_PROPERTY(QString pass READ pass WRITE setPass NOTIFY configChanged)
    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY configChanged)
    Q_PROPERTY(bool shuffle READ shuffle WRITE setShuffle NOTIFY configChanged)
    Q_PROPERTY(bool useActionButtons READ useActionButtons WRITE setUseActionButtons NOTIFY configChanged)

    Q_PROPERTY(bool useSchedule READ useSchedule WRITE setUseSchedule NOTIFY configChanged)
    Q_PROPERTY(QString wakeTime READ wakeTimeStr WRITE setWakeTimeStr NOTIFY configChanged)
    Q_PROPERTY(QString sleepTime READ sleepTimeStr WRITE setSleepTimeStr NOTIFY configChanged)

    Q_PROPERTY(bool useGuest READ useGuest WRITE setUseGuest NOTIFY configChanged)
    Q_PROPERTY(QString smbVers READ smbVers WRITE setSmbVers NOTIFY configChanged)
    Q_PROPERTY(bool useRtsp READ useRtsp WRITE setUseRtsp NOTIFY configChanged)
    Q_PROPERTY(QString rtspUrl READ rtspUrl WRITE setRtspUrl NOTIFY configChanged)

    Q_PROPERTY(int rtspState READ rtspState NOTIFY rtspStateChanged)
    Q_PROPERTY(QString rtspErrorMsg READ rtspErrorMsg NOTIFY rtspErrorMsgChanged)

    // Camera 2
    Q_PROPERTY(bool useRtsp2 READ useRtsp2 WRITE setUseRtsp2 NOTIFY configChanged)
    Q_PROPERTY(QString rtspUrl2 READ rtspUrl2 WRITE setRtspUrl2 NOTIFY configChanged)
    Q_PROPERTY(int camera2Duration READ camera2Duration WRITE setCamera2Duration NOTIFY configChanged)
    Q_PROPERTY(int rtsp2State READ rtsp2State NOTIFY rtsp2StateChanged)
    Q_PROPERTY(QString rtsp2ErrorMsg READ rtsp2ErrorMsg NOTIFY rtsp2ErrorMsgChanged)

    // Camera 3
    Q_PROPERTY(bool useRtsp3 READ useRtsp3 WRITE setUseRtsp3 NOTIFY configChanged)
    Q_PROPERTY(QString rtspUrl3 READ rtspUrl3 WRITE setRtspUrl3 NOTIFY configChanged)
    Q_PROPERTY(int camera3Duration READ camera3Duration WRITE setCamera3Duration NOTIFY configChanged)
    Q_PROPERTY(int rtsp3State READ rtsp3State NOTIFY rtsp3StateChanged)
    Q_PROPERTY(QString rtsp3ErrorMsg READ rtsp3ErrorMsg NOTIFY rtsp3ErrorMsgChanged)

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
    Q_PROPERTY(qreal signalNetTemperatureOut READ signalNetTemperatureOut NOTIFY signalNetTemperatureOutChanged)
    Q_PROPERTY(bool signalNetTemperatureOutValid READ signalNetTemperatureOutValid NOTIFY signalNetTemperatureOutValidChanged)
    Q_PROPERTY(qreal signalNetHumidity READ signalNetHumidity NOTIFY signalNetHumidityChanged)
    Q_PROPERTY(bool signalNetHumidityValid READ signalNetHumidityValid NOTIFY signalNetHumidityValidChanged)
    Q_PROPERTY(int signalNetCo2 READ signalNetCo2 NOTIFY signalNetCo2Changed)
    Q_PROPERTY(bool signalNetCo2Valid READ signalNetCo2Valid NOTIFY signalNetCo2ValidChanged)
    Q_PROPERTY(int signalNetDust READ signalNetDust NOTIFY signalNetDustChanged)
    Q_PROPERTY(bool signalNetDustValid READ signalNetDustValid NOTIFY signalNetDustValidChanged)
    Q_PROPERTY(qreal signalNetVar READ signalNetVar NOTIFY signalNetVarChanged)
    Q_PROPERTY(bool signalNetVarValid READ signalNetVarValid NOTIFY signalNetVarValidChanged)
    Q_PROPERTY(bool signalNetUseUdp READ signalNetUseUdp WRITE setSignalNetUseUdp NOTIFY configChanged)
    Q_PROPERTY(int signalNetUdpLocalPort READ signalNetUdpLocalPort WRITE setSignalNetUdpLocalPort NOTIFY configChanged)
    Q_PROPERTY(QString signalNetUdpKey READ signalNetUdpKey WRITE setSignalNetUdpKey NOTIFY configChanged)
    Q_PROPERTY(int cameraDuration READ cameraDuration WRITE setCameraDuration NOTIFY configChanged)

    Q_PROPERTY(int brightness READ brightness WRITE setBrightness NOTIFY configChanged)
    Q_PROPERTY(bool backlightAvailable READ backlightAvailable)


public:
    explicit PhotoFrameBackend(QObject* parent = nullptr);
    ~PhotoFrameBackend();
    void setImageProvider(ImageProvider* p) { m_imageProvider = p; }

    QString currentTime() const;
    QString currentDate() const;
    QString currentImagePath() const;
    QString currentFileName() const;
    QString currentFileDate() const;
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
    bool useActionButtons() const;
    void setUseActionButtons(bool v);

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

    bool useRtsp2() const;
    void setUseRtsp2(bool v);
    QString rtspUrl2() const;
    void setRtspUrl2(const QString& v);
    int camera2Duration() const;
    void setCamera2Duration(int v);

    bool useRtsp3() const;
    void setUseRtsp3(bool v);
    QString rtspUrl3() const;
    void setRtspUrl3(const QString& v);
    int camera3Duration() const;
    void setCamera3Duration(int v);

    Q_INVOKABLE void nextSlide();
    Q_INVOKABLE void prevSlide();
    Q_INVOKABLE void firstSlide();
    Q_INVOKABLE void lastSlide();
    Q_INVOKABLE void toggleSlideshow();
    Q_INVOKABLE void saveSettings();
    Q_INVOKABLE void connectAndScan();
    Q_INVOKABLE void reconnectRtsp();
    Q_INVOKABLE void reconnectRtsp2();
    Q_INVOKABLE void reconnectRtsp3();
    Q_INVOKABLE void fallbackToPhotos();
    Q_INVOKABLE QStringList tasks() const;

    bool isSleeping() const { return m_isSleeping; }
    int playlistSize() const { return m_playlist.size(); }

    // Called by QML MediaPlayer to report state back to backend
    Q_INVOKABLE void onRtspPlaying();
    Q_INVOKABLE void onRtspError(const QString& msg);
    Q_INVOKABLE void stopRtsp();
    Q_INVOKABLE void onRtsp2Playing();
    Q_INVOKABLE void onRtsp2Error(const QString& msg);
    Q_INVOKABLE void stopRtsp2();
    Q_INVOKABLE void onRtsp3Playing();
    Q_INVOKABLE void onRtsp3Error(const QString& msg);
    Q_INVOKABLE void stopRtsp3();

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
    qreal signalNetTemperatureOut() const;
    bool signalNetTemperatureOutValid() const;
    qreal signalNetHumidity() const;
    bool signalNetHumidityValid() const;
    int signalNetCo2() const;
    bool signalNetCo2Valid() const;
    int signalNetDust() const;
    bool signalNetDustValid() const;
    qreal signalNetVar() const;
    bool signalNetVarValid() const;
    int cameraDuration() const;
    void setCameraDuration(int v);
    int signalNetDeviceAddress() const;
    void setSignalNetDeviceAddress(int v);

    int brightness() const;
    void setBrightness(int v);
    bool backlightAvailable() const;

    Q_INVOKABLE void connectSignalNet();
    Q_INVOKABLE void disconnectSignalNet();
    Q_INVOKABLE void clearSignalNetAlert();
    Q_INVOKABLE void sendAction1();
    Q_INVOKABLE void sendAction2();

enum RtspState { RtspIdle = 0, RtspConnecting, RtspPlaying, RtspError };
    Q_ENUM(RtspState)

    int rtspState() const;
    QString rtspErrorMsg() const;
    int rtsp2State() const;
    QString rtsp2ErrorMsg() const;
    int rtsp3State() const;
    QString rtsp3ErrorMsg() const;

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

    void rtsp2Play(const QString& url);
    void rtsp2StopPlayer();
    void rtsp2ShowOverlay(const QString& msg);
    void rtsp2HideOverlay();
    void rtsp2StateChanged();
    void rtsp2ErrorMsgChanged();

    void rtsp3Play(const QString& url);
    void rtsp3StopPlayer();
    void rtsp3ShowOverlay(const QString& msg);
    void rtsp3HideOverlay();
    void rtsp3StateChanged();
    void rtsp3ErrorMsgChanged();

    // SignalNet signals
    void signalNetConnectedChanged();
    void signalNetTemperatureChanged();
    void signalNetAlertChanged();
    void signalNetAlertSeverityChanged();
    void signalNetTemperatureValidChanged();
    void signalNetTemperatureOutChanged();
    void signalNetTemperatureOutValidChanged();
    void signalNetHumidityChanged();
    void signalNetHumidityValidChanged();
    void signalNetCo2Changed();
    void signalNetCo2ValidChanged();
    void signalNetDustChanged();
    void signalNetDustValidChanged();
    void signalNetVarChanged();
    void signalNetVarValidChanged();

private slots:
    void onTick();
    void onScanFinished(const QStringList& list);
    void onRtspRetryTimeout();
    void onRtspFallbackTimeout();
    void onCameraTimeout();
    void onRtsp2RetryTimeout();
    void onRtsp2FallbackTimeout();
    void onCamera2Timeout();
    void onRtsp3RetryTimeout();
    void onRtsp3FallbackTimeout();
    void onCamera3Timeout();

private:
    void checkSchedule();
    void setSleepMode(bool sleep);

    void applyBacklight(int pct);
    void initBacklight();

    void startRtsp();
    void setRtspState(RtspState s);
    void startRtsp2();
    void setRtsp2State(RtspState s);
    void startRtsp3();
    void setRtsp3State(RtspState s);

    void resumeSlideshow();
    void stopAllCameras();
    void forceStopRtsp();
    void forceStopRtsp2();
    void forceStopRtsp3();

    QTimer* m_tickTimer;
    QTimer* m_slideshowTimer;
    QTimer* m_rtspRetryTimer;
    QTimer* m_rtspFallbackTimer;
    QTimer* m_rtsp2RetryTimer = nullptr;
    QTimer* m_rtsp2FallbackTimer = nullptr;
    QTimer* m_rtsp3RetryTimer = nullptr;
    QTimer* m_rtsp3FallbackTimer = nullptr;
    SmbConfig m_config;
    QStringList m_playlist;
    int m_idx = 0;
    int m_pageIndex = 0;
    bool m_isSleeping = false;
    bool m_destroyed = false;
    bool m_scanning = false;
    QString m_currentImagePath;
    QString m_currentFileName;
    QString m_currentFileDate;
    int m_imageCounter = 0;
    static const int kMaxConsecutiveFails = 50;
    RtspState m_rtspState = RtspIdle;
    int m_rtspRetryCount = 0;
    int m_rtspSession = 0;
    static const int kMaxRtspRetries = 3;
    QString m_rtspErrorMsg;

    RtspState m_rtsp2State = RtspIdle;
    int m_rtsp2RetryCount = 0;
    int m_rtsp2Session = 0;
    QString m_rtsp2ErrorMsg;

    RtspState m_rtsp3State = RtspIdle;
    int m_rtsp3RetryCount = 0;
    int m_rtsp3Session = 0;
    QString m_rtsp3ErrorMsg;

    ImageProvider* m_imageProvider = nullptr;
    SignalNet* m_signalNet = nullptr;
    QSoundEffect* m_alertSound = nullptr;
    QTimer* m_cameraTimer = nullptr;
    QTimer* m_camera2Timer = nullptr;
    QTimer* m_camera3Timer = nullptr;

    int m_backlightMax = 255;
    bool m_backlightAvailable = false;
    static const int kSleepBacklight = 0; // выключить подсветку полностью (не прятать изображение)

};

#endif // PHOTOFRAME_H

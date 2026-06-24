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

#include "config.h"

class ImageProvider : public QQuickImageProvider {
public:
    ImageProvider();
    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;
    void setCurrentImage(const QImage& img);
private:
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

    Q_PROPERTY(bool useRtsp READ useRtsp WRITE setUseRtsp NOTIFY configChanged)
    Q_PROPERTY(QString rtspUrl READ rtspUrl WRITE setRtspUrl NOTIFY configChanged)

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

    bool useRtsp() const;
    void setUseRtsp(bool v);
    QString rtspUrl() const;
    void setRtspUrl(const QString& v);

    Q_INVOKABLE void nextSlide();
    Q_INVOKABLE void prevSlide();
    Q_INVOKABLE void toggleSlideshow();
    Q_INVOKABLE void saveSettings();
    Q_INVOKABLE void connectAndScan();
    Q_INVOKABLE QStringList tasks() const;

signals:
    void tick();
    void imageChanged();
    void pageIndexChanged();
    void configChanged();
    void sleepChanged(bool sleeping);
    void showSettingsPage();
    void rtspStarted(const QString& url);
    void rtspStopped();

private slots:
    void onTick();
    void onScanFinished(const QStringList& list);

private:
    void checkSchedule();
    void setSleepMode(bool sleep);
    void loadImage(const QString& path);

    QTimer* m_tickTimer;
    QTimer* m_slideshowTimer;
    SmbConfig m_config;
    QStringList m_playlist;
    int m_idx = 0;
    int m_pageIndex = 0;
    bool m_isSleeping = false;
    bool m_destroyed = false;
    QString m_currentImagePath;
    ImageProvider* m_imageProvider = nullptr;
};

#endif // PHOTOFRAME_H

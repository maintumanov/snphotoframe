#ifndef RTSPVIEWER_H
#define RTSPVIEWER_H

#include <QWidget>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QString>

class RtspViewer : public QWidget {
    Q_OBJECT
public:
    explicit RtspViewer(QWidget* parent = nullptr);
    ~RtspViewer();
    
    void setUrl(const QString& url);
    void play();
    void stop();
    
protected:
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

private:
    QMediaPlayer* m_player;
    QVideoWidget* m_videoWidget;
    QString m_url;
};

#endif // RTSPVIEWER_H

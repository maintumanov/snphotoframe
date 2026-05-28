#include "rtspviewer.h"
#include <QVBoxLayout>
#include <QDebug>

RtspViewer::RtspViewer(QWidget* parent) : QWidget(parent), m_player(nullptr), m_videoWidget(nullptr) {
    setAttribute(Qt::WA_OpaquePaintEvent);
    setStyleSheet("background-color: black;");
}

RtspViewer::~RtspViewer() {
    stop();
}

void RtspViewer::setUrl(const QString& url) {
    m_url = url;
}

void RtspViewer::play() {
    if (m_url.isEmpty()) {
        qWarning() << "RTSP URL is not set";
        return;
    }
    
    // Очищаем предыдущий плеер если есть
    if (m_player) {
        stop();
    }
    
    // Создаем новый медиаплеер и видео виджет
    m_player = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
    m_videoWidget = new QVideoWidget(this);
    m_videoWidget->setStyleSheet("background-color: black;");
    
    // Настраиваем layout для размещения видео виджета
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_videoWidget);
    setLayout(layout);
    
    m_player->setVideoOutput(m_videoWidget);
    m_player->setMedia(QUrl(m_url));
    
    connect(m_player, &QMediaPlayer::error, this, [](QMediaPlayer::Error error) {
        qWarning() << "MediaPlayer error:" << error;
    });
    
    connect(m_player, &QMediaPlayer::stateChanged, this, [](QMediaPlayer::State state) {
        qDebug() << "MediaPlayer state changed:" << state;
    });
    
    m_player->play();
    m_videoWidget->show();
}

void RtspViewer::stop() {
    if (m_player) {
        m_player->stop();
        m_player->deleteLater();
        m_player = nullptr;
    }
    if (m_videoWidget) {
        m_videoWidget->hide();
        m_videoWidget->deleteLater();
        m_videoWidget = nullptr;
    }
}

void RtspViewer::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    if (!m_url.isEmpty() && !m_player) {
        play();
    }
}

void RtspViewer::hideEvent(QHideEvent* event) {
    QWidget::hideEvent(event);
    // Останавливаем воспроизведение при скрытии для экономии ресурсов
    stop();
}

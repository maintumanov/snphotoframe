#include "imagedisplay.h"
#include <QImage>

ImageDisplay::ImageDisplay(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_OpaquePaintEvent);
}

void ImageDisplay::setImage(const QImage& img) {
    m_pix = QPixmap::fromImage(img);
    update();
}

void ImageDisplay::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.fillRect(rect(), Qt::black);
    if (m_pix.isNull())
        return;
    QSize s = m_pix.size().scaled(size(), Qt::KeepAspectRatio);
    p.drawPixmap((width() - s.width()) / 2, (height() - s.height()) / 2,
                 m_pix.scaled(s, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

#ifndef IMAGEDISPLAY_H
#define IMAGEDISPLAY_H

#include <QWidget>
#include <QPixmap>
#include <QPaintEvent>
#include <QPainter>

class ImageDisplay : public QWidget {
    Q_OBJECT
public:
    explicit ImageDisplay(QWidget* parent = nullptr);
    void setImage(const QImage& img);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    QPixmap m_pix;
};

#endif // IMAGEDISPLAY_H

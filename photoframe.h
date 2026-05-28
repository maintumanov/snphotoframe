#ifndef PHOTOFRAME_H
#define PHOTOFRAME_H

#include <QMainWindow>
#include <QStackedLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QStringList>

#include "config.h"

class ImageDisplay;

class PhotoFrame : public QMainWindow {
    Q_OBJECT
public:
    PhotoFrame();

private slots:
    void nextSlide();
    void prevSlide();
    void showSettings();
    void updateClock();
    void connectAndScan();
    void onScanFinished(const QStringList& list);

protected:
    void mouseMoveEvent(QMouseEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;

private:
    void setupUi();
    void raiseOverlays();
    void checkSchedule();
    void setSleepMode(bool sleep);

    QStackedLayout* m_stack;
    ImageDisplay *m_viewA, *m_viewB;
    QLabel *m_clock, *m_date, *m_photoDate;
    QPushButton* m_setBtn;
    QWidget* m_sleepScreen;

    QTimer *m_timer, *m_clockTimer, *m_uiTimer;
    SmbConfig m_config;
    QStringList m_playlist;
    int m_idx = 0;
    QPoint m_startPos;
    bool m_isSleeping = false;
};

#endif // PHOTOFRAME_H

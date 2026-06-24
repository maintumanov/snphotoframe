#ifndef PHOTOFRAME_H
#define PHOTOFRAME_H

#include <QMainWindow>
#include <QStackedLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QStringList>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QTimeEdit>
#include <QListWidget>

#include "config.h"

class ImageDisplay;
class RtspViewer;

class PhotoFrame : public QMainWindow {
    Q_OBJECT
public:
    PhotoFrame();

private slots:
    void nextSlide();
    void prevSlide();
    void updateClock();
    void connectAndScan();
    void onScanFinished(const QStringList& list);
    void switchToPage(int page);
    void applySettings();

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;

private:
    void setupUi();
    void setupControlBar();
    QWidget* buildSettingsPage();
    QWidget* buildTasksPage();
    QWidget* buildCalendarPage();
    void checkSchedule();
    void setSleepMode(bool sleep);
    void startRtspStream();
    void stopRtspStream();
    void refreshTasks();

    QStackedLayout* m_pages;
    ImageDisplay *m_viewA, *m_viewB;
    RtspViewer* m_rtspViewer;
    QWidget* m_sleepScreen;

    QWidget* m_controlBar;
    QPushButton* m_clockBtn;
    QPushButton* m_dateBtn;
    QPushButton* m_videoBtn;
    QPushButton* m_equalizerBtn;
    QPushButton* m_tasksBtn;
    QPushButton* m_calendarBtn;
    QPushButton* m_navBtns[4];

    QTimer *m_timer, *m_clockTimer;
    SmbConfig m_config;
    QStringList m_playlist;
    int m_idx = 0;
    QPoint m_startPos;
    bool m_isSleeping = false;
    bool m_showingRtsp = false;
    int m_currentPage = 0;

    QLineEdit *m_srvEdit, *m_shrEdit, *m_usrEdit, *m_pswEdit;
    QSpinBox *m_intervalEdit;
    QCheckBox *m_shuffleCheck;
    QCheckBox *m_scheduleCheck;
    QTimeEdit *m_wakeEdit, *m_sleepTimeEdit;
    QCheckBox *m_rtspCheck;
    QLineEdit *m_rtspUrlEdit;
    QListWidget* m_taskList;
};

#endif // PHOTOFRAME_H

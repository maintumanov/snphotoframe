#include "photoframe.h"
#include "imagedisplay.h"
#include "rtspviewer.h"
#include "playlistmanager.h"

#include <QtWidgets>
#include <QtConcurrent>
#include <QProcess>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QDirIterator>
#include <QDateTime>
#include <QImageReader>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QCalendarWidget>
#include <QListWidget>
#include <QScrollArea>
#include <QFormLayout>
#include <algorithm>
#include <random>

static const QString btnBase() {
    return
        "QPushButton { background: rgba(0,0,0,0.3); border: 2px solid white; border-radius: 14px; color: white; font-size: 14pt; font-weight: bold; }"
        "QPushButton:hover { background: rgba(255,255,255,0.2); }"
        "QPushButton:pressed { background: rgba(255,255,255,0.35); }";
}

static const QString btnActive() {
    return
        "QPushButton { background: rgba(255,255,255,0.3); border: 2px solid white; border-radius: 14px; color: white; font-size: 14pt; font-weight: bold; }";
}

static const QString darkPageStyle() {
    return
        "QLabel { color: white; }"
        "QLineEdit { background: #222; color: white; border: 1px solid #555; border-radius: 6px; padding: 8px; font-size: 13pt; }"
        "QLineEdit:focus { border-color: #888; }"
        "QSpinBox { background: #222; color: white; border: 1px solid #555; border-radius: 6px; padding: 8px; font-size: 13pt; }"
        "QTimeEdit { background: #222; color: white; border: 1px solid #555; border-radius: 6px; padding: 8px; font-size: 13pt; }"
        "QCheckBox { color: white; font-size: 13pt; spacing: 8px; }"
        "QCheckBox::indicator { width: 22px; height: 22px; border: 2px solid #888; border-radius: 5px; background: #222; }"
        "QCheckBox::indicator:checked { background: #5a5; border-color: #5a5; }"
        "QListWidget { background: #111; color: white; border: 1px solid #333; font-size: 14pt; }"
        "QListWidget::item { padding: 12px; border-bottom: 1px solid #222; }"
        "QListWidget::item:selected { background: #333; }"
        "QScrollArea { border: none; background: black; }"
        "QScrollBar:vertical { background: #111; width: 8px; }"
        "QScrollBar::handle:vertical { background: #555; border-radius: 4px; min-height: 30px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }"
        "QCalendarWidget { background: black; color: white; }"
        "QCalendarWidget QWidget { alternate-background-color: #1a1a1a; color: white; }"
        "QCalendarWidget QAbstractItemView { color: white; background: #111; selection-background-color: #555; selection-color: white; }"
        "QCalendarWidget QToolButton { color: white; background: #333; border-radius: 6px; padding: 6px 12px; font-size: 13pt; }"
        "QCalendarWidget QToolButton:hover { background: #555; }"
        "QCalendarWidget QToolButton:pressed { background: #666; }"
        "QCalendarWidget QSpinBox { background: #222; color: white; border: 1px solid #555; border-radius: 4px; padding: 4px; }"
        "QCalendarWidget QAbstractItemView:enabled { font-size: 13pt; }"
        "QCalendarWidget QHeaderView::section { color: white; background: #222; border: 1px solid #333; padding: 6px; font-size: 13pt; }";
}

PhotoFrame::PhotoFrame() {
    m_config.load();

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &PhotoFrame::nextSlide);

    m_clockTimer = new QTimer(this);
    connect(m_clockTimer, &QTimer::timeout, this, &PhotoFrame::updateClock);

    setupUi();

    setWindowFlags(Qt::FramelessWindowHint);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    m_clockTimer->start(1000);
    updateClock();

    if (m_config.useRtsp && !m_config.rtspUrl.isEmpty()) {
        startRtspStream();
    } else {
        m_playlist = PlaylistManager::load();
        if (!m_playlist.isEmpty()) {
            if (m_config.shuffle)
                std::shuffle(m_playlist.begin(), m_playlist.end(), std::mt19937(std::random_device()()));
            checkSchedule();
            if (!m_isSleeping) {
                m_timer->start(m_config.interval);
                QTimer::singleShot(500, this, &PhotoFrame::nextSlide);
            }
        } else {
            QTimer::singleShot(1000, this, &PhotoFrame::connectAndScan);
        }
    }

    showFullScreen();
}

void PhotoFrame::setupUi() {
    QWidget* central = new QWidget(this);
    central->setMouseTracking(true);
    central->setStyleSheet("background: black;");
    setCentralWidget(central);

    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_pages = new QStackedLayout;

    QWidget* photoPage = new QWidget;
    photoPage->setMouseTracking(true);
    {
        QVBoxLayout* l = new QVBoxLayout(photoPage);
        l->setContentsMargins(0, 0, 0, 0);
        QWidget* stackW = new QWidget(photoPage);
        stackW->setMouseTracking(true);
        QStackedLayout* sl = new QStackedLayout(stackW);
        sl->setStackingMode(QStackedLayout::StackAll);
        m_viewA = new ImageDisplay;
        m_viewB = new ImageDisplay;
        m_rtspViewer = new RtspViewer;
        m_rtspViewer->hide();
        sl->addWidget(m_viewA);
        sl->addWidget(m_viewB);
        sl->addWidget(m_rtspViewer);
        for (auto* v : {m_viewA, m_viewB}) {
            auto* eff = new QGraphicsOpacityEffect(v);
            eff->setOpacity(v == m_viewA ? 1.0 : 0.0);
            v->setGraphicsEffect(eff);
        }
        l->addWidget(stackW);
    }
    m_pages->addWidget(photoPage);

    m_pages->addWidget(buildSettingsPage());
    m_pages->addWidget(buildTasksPage());
    m_pages->addWidget(buildCalendarPage());

    mainLayout->addLayout(m_pages, 1);

    setupControlBar();
    mainLayout->addWidget(m_controlBar);

    m_sleepScreen = new QWidget(central);
    m_sleepScreen->setStyleSheet("background-color: black;");
    m_sleepScreen->hide();
}

QWidget* PhotoFrame::buildSettingsPage() {
    QWidget* page = new QWidget;
    page->setStyleSheet("background: black;");

    QScrollArea* scroll = new QScrollArea(page);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("QScrollArea { background: black; border: none; }");

    QWidget* content = new QWidget;
    content->setStyleSheet("background: black;");

    QVBoxLayout* contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(50, 30, 50, 20);
    contentLayout->setSpacing(0);

    QLabel* title = new QLabel(QString::fromUtf8("\u041d\u0430\u0441\u0442\u0440\u043e\u0439\u043a\u0438"));
    title->setStyleSheet("font-size: 22pt; font-weight: bold; color: white; padding-bottom: 20px;");
    contentLayout->addWidget(title);

    QString lblStyle = "color: white; font-size: 14pt; font-weight: bold;";
    QString inputStyle = "QLineEdit { background: #222; color: white; border: 1px solid #555; border-radius: 8px; padding: 10px; font-size: 14pt; }"
                         "QLineEdit:focus { border-color: #aaa; }";
    QString spinStyle = "QSpinBox { background: #222; color: white; border: 1px solid #555; border-radius: 8px; padding: 10px; font-size: 14pt; }";
    QString timeStyle = "QTimeEdit { background: #222; color: white; border: 1px solid #555; border-radius: 8px; padding: 10px; font-size: 14pt; }";
    QString chkStyle = "QCheckBox { color: white; font-size: 14pt; spacing: 10px; }"
                       "QCheckBox::indicator { width: 24px; height: 24px; border: 2px solid #888; border-radius: 6px; background: #222; }"
                       "QCheckBox::indicator:checked { background: #5a5; border-color: #5a5; }";

    QFormLayout* f = new QFormLayout;
    f->setVerticalSpacing(16);
    f->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_srvEdit = new QLineEdit(m_config.server);
    m_shrEdit = new QLineEdit(m_config.share);
    m_usrEdit = new QLineEdit(m_config.user);
    m_pswEdit = new QLineEdit(m_config.pass);
    m_pswEdit->setEchoMode(QLineEdit::Password);
    for (auto* w : {m_srvEdit, m_shrEdit, m_usrEdit, m_pswEdit}) w->setStyleSheet(inputStyle);

    m_intervalEdit = new QSpinBox;
    m_intervalEdit->setRange(2, 3600);
    m_intervalEdit->setValue(m_config.interval / 1000);
    m_intervalEdit->setStyleSheet(spinStyle);

    m_shuffleCheck = new QCheckBox(QString::fromUtf8("\u0421\u043b\u0443\u0447\u0430\u0439\u043d\u044b\u0439 \u043f\u043e\u0440\u044f\u0434\u043e\u043a"));
    m_shuffleCheck->setChecked(m_config.shuffle);
    m_shuffleCheck->setStyleSheet(chkStyle);

    m_scheduleCheck = new QCheckBox(QString::fromUtf8("\u0412\u043a\u043b\u044e\u0447\u0438\u0442\u044c \u0441\u043f\u044f\u0449\u0438\u0439 \u0440\u0435\u0436\u0438\u043c"));
    m_scheduleCheck->setChecked(m_config.useSchedule);
    m_scheduleCheck->setStyleSheet(chkStyle);
    m_wakeEdit = new QTimeEdit(m_config.wakeTime);
    m_wakeEdit->setDisplayFormat("HH:mm");
    m_wakeEdit->setStyleSheet(timeStyle);
    m_sleepTimeEdit = new QTimeEdit(m_config.sleepTime);
    m_sleepTimeEdit->setDisplayFormat("HH:mm");
    m_sleepTimeEdit->setStyleSheet(timeStyle);

    m_rtspCheck = new QCheckBox(QString::fromUtf8("\u041f\u043e\u043a\u0430\u0437\u044b\u0432\u0430\u0442\u044c \u0432\u0438\u0434\u0435\u043e \u0441 RTSP \u043a\u0430\u043c\u0435\u0440\u044b"));
    m_rtspCheck->setChecked(m_config.useRtsp);
    m_rtspCheck->setStyleSheet(chkStyle);
    m_rtspUrlEdit = new QLineEdit(m_config.rtspUrl);
    m_rtspUrlEdit->setPlaceholderText("rtsp://user:pass@ip:port/stream");
    m_rtspUrlEdit->setStyleSheet(inputStyle);

    auto addLabel = [&](const QString& text) {
        QLabel* lbl = new QLabel(text);
        lbl->setStyleSheet(lblStyle);
        return lbl;
    };

    f->addRow(addLabel(QString::fromUtf8("\u0421\u0435\u0440\u0432\u0435\u0440/IP:")), m_srvEdit);
    f->addRow(addLabel(QString::fromUtf8("\u041f\u0430\u043f\u043a\u0430:")), m_shrEdit);
    f->addRow(addLabel(QString::fromUtf8("\u041b\u043e\u0433\u0438\u043d:")), m_usrEdit);
    f->addRow(addLabel(QString::fromUtf8("\u041f\u0430\u0440\u043e\u043b\u044c:")), m_pswEdit);
    f->addRow(addLabel(QString::fromUtf8("\u0418\u043d\u0442\u0435\u0440\u0432\u0430\u043b (\u0441\u0435\u043a):")), m_intervalEdit);
    f->addRow(QString(), m_shuffleCheck);

    contentLayout->addLayout(f);

    QLabel* schedTitle = new QLabel(QString::fromUtf8("\u0420\u0430\u0441\u043f\u0438\u0441\u0430\u043d\u0438\u0435"));
    schedTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: white; padding-top: 20px; padding-bottom: 8px;");
    contentLayout->addWidget(schedTitle);

    QFormLayout* f2 = new QFormLayout;
    f2->setVerticalSpacing(16);
    f2->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    f2->addRow(QString(), m_scheduleCheck);
    f2->addRow(addLabel(QString::fromUtf8("\u0412\u0440\u0435\u043c\u044f \u043f\u0440\u043e\u0431\u0443\u0436\u0434\u0435\u043d\u0438\u044f:")), m_wakeEdit);
    f2->addRow(addLabel(QString::fromUtf8("\u0412\u0440\u0435\u043c\u044f \u0441\u043d\u0430:")), m_sleepTimeEdit);
    contentLayout->addLayout(f2);

    QLabel* rtspTitle = new QLabel("RTSP");
    rtspTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: white; padding-top: 20px; padding-bottom: 8px;");
    contentLayout->addWidget(rtspTitle);

    QFormLayout* f3 = new QFormLayout;
    f3->setVerticalSpacing(16);
    f3->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    f3->addRow(QString(), m_rtspCheck);
    f3->addRow(addLabel(QString::fromUtf8("URL \u043a\u0430\u043c\u0435\u0440\u044b:")), m_rtspUrlEdit);
    contentLayout->addLayout(f3);

    contentLayout->addStretch();

    scroll->setWidget(content);

    QVBoxLayout* outer = new QVBoxLayout(page);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->addWidget(scroll, 1);

    QString btnStyle =
        "QPushButton { background: rgba(255,255,255,0.15); color: white; border: 2px solid white; "
        "border-radius: 14px; padding: 12px 40px; font-size: 14pt; font-weight: bold; }"
        "QPushButton:hover { background: rgba(255,255,255,0.3); }"
        "QPushButton:pressed { background: rgba(255,255,255,0.45); }";

    QHBoxLayout* btnRow = new QHBoxLayout;
    btnRow->setContentsMargins(50, 0, 50, 20);

    QPushButton* exitBtn = new QPushButton(QString::fromUtf8("\u0412\u044b\u0445\u043e\u0434"));
    exitBtn->setStyleSheet(btnStyle);
    connect(exitBtn, &QPushButton::clicked, [this]() { switchToPage(0); });
    btnRow->addWidget(exitBtn);

    btnRow->addStretch();

    QPushButton* saveBtn = new QPushButton(QString::fromUtf8("\u0421\u043e\u0445\u0440\u0430\u043d\u0438\u0442\u044c"));
    saveBtn->setStyleSheet(btnStyle);
    connect(saveBtn, &QPushButton::clicked, this, &PhotoFrame::applySettings);
    btnRow->addWidget(saveBtn);

    outer->addLayout(btnRow);
    return page;
}

QWidget* PhotoFrame::buildTasksPage() {
    QWidget* page = new QWidget;
    page->setStyleSheet("background: black;");

    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(50, 30, 50, 30);
    layout->setSpacing(20);

    QLabel* title = new QLabel(QString::fromUtf8("\u0422\u0435\u043a\u0443\u0449\u0438\u0435 \u0437\u0430\u0434\u0430\u0447\u0438"));
    title->setStyleSheet("font-size: 22pt; font-weight: bold; color: white;");
    layout->addWidget(title);

    m_taskList = new QListWidget;
    m_taskList->setStyleSheet(
        "QListWidget { background: #111; color: white; border: 1px solid #333; font-size: 14pt; }"
        "QListWidget::item { padding: 12px; border-bottom: 1px solid #222; }"
        "QListWidget::item:selected { background: #333; }");
    layout->addWidget(m_taskList, 1);
    refreshTasks();
    return page;
}

QWidget* PhotoFrame::buildCalendarPage() {
    QWidget* page = new QWidget;
    page->setStyleSheet("background: black;");

    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(50, 30, 50, 30);
    layout->setSpacing(20);

    QLabel* title = new QLabel(QString::fromUtf8("\u041a\u0430\u043b\u0435\u043d\u0434\u0430\u0440\u044c"));
    title->setStyleSheet("font-size: 22pt; font-weight: bold; color: white;");
    layout->addWidget(title);

    QCalendarWidget* calendar = new QCalendarWidget;
    calendar->setGridVisible(true);
    calendar->setStyleSheet(darkPageStyle());
    layout->addWidget(calendar, 1);
    return page;
}

void PhotoFrame::refreshTasks() {
    m_taskList->clear();
    QFile file("tasks.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (!line.isEmpty()) {
                QListWidgetItem* item = new QListWidgetItem(line);
                item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
                item->setCheckState(Qt::Unchecked);
                m_taskList->addItem(item);
            }
        }
        file.close();
    }
    if (m_taskList->count() == 0) {
        QListWidgetItem* empty = new QListWidgetItem(QString::fromUtf8("\u041d\u0435\u0442 \u0437\u0430\u0434\u0430\u0447"));
        empty->setFlags(empty->flags() & ~Qt::ItemIsSelectable);
        m_taskList->addItem(empty);
    }
}

void PhotoFrame::setupControlBar() {
    m_controlBar = new QWidget;
    m_controlBar->setFixedHeight(100);

    QHBoxLayout* layout = new QHBoxLayout(m_controlBar);
    layout->setContentsMargins(8, 0, 8, 40);
    layout->setSpacing(16);
    layout->addStretch();

    m_clockBtn = new QPushButton;
    m_clockBtn->setFixedSize(84, 56);
    m_clockBtn->setStyleSheet(btnBase());
    m_clockBtn->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    layout->addWidget(m_clockBtn);

    m_dateBtn = new QPushButton;
    m_dateBtn->setFixedSize(84, 56);
    m_dateBtn->setStyleSheet(btnBase());
    m_dateBtn->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    layout->addWidget(m_dateBtn);

    m_videoBtn = new QPushButton;
    m_videoBtn->setFixedSize(56, 56);
    m_videoBtn->setStyleSheet(btnBase());
    m_videoBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPlay));
    layout->addWidget(m_videoBtn);

    m_equalizerBtn = new QPushButton;
    m_equalizerBtn->setFixedSize(56, 56);
    m_equalizerBtn->setStyleSheet(btnBase());
    m_equalizerBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    layout->addWidget(m_equalizerBtn);

    m_tasksBtn = new QPushButton;
    m_tasksBtn->setFixedSize(56, 56);
    m_tasksBtn->setStyleSheet(btnBase());
    m_tasksBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogApplyButton));
    layout->addWidget(m_tasksBtn);

    m_calendarBtn = new QPushButton;
    m_calendarBtn->setFixedSize(56, 56);
    m_calendarBtn->setStyleSheet(btnBase());
    m_calendarBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileDialogInfoView));
    layout->addWidget(m_calendarBtn);

    layout->addStretch();

    m_navBtns[0] = m_videoBtn;
    m_navBtns[1] = m_equalizerBtn;
    m_navBtns[2] = m_tasksBtn;
    m_navBtns[3] = m_calendarBtn;

    connect(m_videoBtn, &QPushButton::clicked, [this]() { switchToPage(m_currentPage == 0 ? -1 : 0); });
    connect(m_equalizerBtn, &QPushButton::clicked, [this]() { switchToPage(m_currentPage == 1 ? -1 : 1); });
    connect(m_tasksBtn, &QPushButton::clicked, [this]() {
        if (m_currentPage == 2) { switchToPage(-1); } else { refreshTasks(); switchToPage(2); }
    });
    connect(m_calendarBtn, &QPushButton::clicked, [this]() { switchToPage(m_currentPage == 3 ? -1 : 3); });
}

void PhotoFrame::switchToPage(int page) {
    if (page == -1) page = 0;
    if (page == m_currentPage) return;
    if (page < 0 || page > 3) return;

    m_currentPage = page;
    m_pages->setCurrentIndex(page);

    for (int i = 0; i < 4; i++)
        m_navBtns[i]->setStyleSheet(i == page ? btnActive() : btnBase());
}

void PhotoFrame::applySettings() {
    m_config.server = m_srvEdit->text();
    m_config.share = m_shrEdit->text();
    m_config.user = m_usrEdit->text();
    m_config.pass = m_pswEdit->text();
    m_config.interval = m_intervalEdit->value() * 1000;
    m_config.shuffle = m_shuffleCheck->isChecked();
    m_config.useSchedule = m_scheduleCheck->isChecked();
    m_config.wakeTime = m_wakeEdit->time();
    m_config.sleepTime = m_sleepTimeEdit->time();
    m_config.useRtsp = m_rtspCheck->isChecked();
    m_config.rtspUrl = m_rtspUrlEdit->text();
    m_config.save();
    PlaylistManager::clear();

    if (m_config.useRtsp && !m_config.rtspUrl.isEmpty())
        startRtspStream();
    else {
        stopRtspStream();
        connectAndScan();
    }
    switchToPage(0);
}

void PhotoFrame::checkSchedule() {
    if (!m_config.useSchedule) return;
    QTime now = QTime::currentTime();
    bool awake = m_config.wakeTime <= m_config.sleepTime
        ? (now >= m_config.wakeTime && now < m_config.sleepTime)
        : (now >= m_config.wakeTime || now < m_config.sleepTime);
    if (awake && m_isSleeping) setSleepMode(false);
    else if (!awake && !m_isSleeping) setSleepMode(true);
}

void PhotoFrame::setSleepMode(bool sleep) {
    m_isSleeping = sleep;
    if (sleep) {
        if (m_timer) m_timer->stop();
        m_sleepScreen->resize(size());
        m_sleepScreen->raise();
        m_sleepScreen->show();
        m_controlBar->hide();
    } else {
        m_sleepScreen->hide();
        m_controlBar->show();
        m_controlBar->raise();
        if (m_timer && !m_playlist.isEmpty()) {
            m_timer->start(m_config.interval);
            nextSlide();
        }
    }
}

void PhotoFrame::updateClock() {
    m_clockBtn->setText(QTime::currentTime().toString("HH:mm"));
    m_dateBtn->setText(QDate::currentDate().toString("dd.MM"));
    if (m_isSleeping) m_sleepScreen->resize(size());
    checkSchedule();
}

void PhotoFrame::connectAndScan() {
    if (m_config.server.isEmpty()) {
        switchToPage(1);
        return;
    }
    QtConcurrent::run([this]() {
#ifdef Q_OS_WIN
        QString path = QString(R"(\\%1\%2)").arg(m_config.server, m_config.share);
        QProcess::execute("net", {"use", path, m_config.pass, QString("/user:%1").arg(m_config.user)});
#else
        QDir().mkpath("/mnt/photoframe");
        QString opts = QString("vers=%1,username=%2,password=%3").arg(m_config.smbVers, m_config.user, m_config.pass);
        QProcess::execute("mount", {"-t", "cifs", QString("//%1/%2").arg(m_config.server, m_config.share), "/mnt/photoframe", "-o", opts});
#endif
        QString scanPath =
#ifdef Q_OS_WIN
            QString(R"(\\%1\%2)").arg(m_config.server, m_config.share);
#else
            "/mnt/photoframe";
#endif
        QStringList files;
        QDirIterator it(scanPath, {"*.jpg", "*.jpeg", "*.png"}, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) files << it.next();
        QMetaObject::invokeMethod(this, "onScanFinished", Q_ARG(QStringList, files));
    });
}

void PhotoFrame::onScanFinished(const QStringList& list) {
    if (list.isEmpty()) { switchToPage(1); return; }
    m_playlist = list;
    PlaylistManager::save(m_playlist);
    if (m_config.shuffle)
        std::shuffle(m_playlist.begin(), m_playlist.end(), std::mt19937(std::random_device()()));
    m_idx = 0;
    if (!m_isSleeping) {
        m_timer->start(m_config.interval);
        nextSlide();
    }
}

void PhotoFrame::nextSlide() {
    if (m_playlist.isEmpty() || m_isSleeping) return;
    QString path = m_playlist[m_idx];
    m_idx = (m_idx + 1) % m_playlist.size();

    QtConcurrent::run([this, path]() {
        QImageReader reader(path);
        reader.setAutoTransform(true);
        QImage img = reader.read();
        if (img.isNull()) { QMetaObject::invokeMethod(this, "nextSlide"); return; }

        QMetaObject::invokeMethod(this, [this, img]() {
            auto* effA = static_cast<QGraphicsOpacityEffect*>(m_viewA->graphicsEffect());
            ImageDisplay* next = (effA && effA->opacity() > 0.5) ? m_viewB : m_viewA;
            ImageDisplay* prev = (next == m_viewA) ? m_viewB : m_viewA;

            next->setImage(img);
            next->raise();
            m_controlBar->raise();

            auto* targetEff = static_cast<QGraphicsOpacityEffect*>(next->graphicsEffect());
            QPropertyAnimation* anim = new QPropertyAnimation(targetEff, "opacity", this);
            anim->setDuration(1500);
            anim->setStartValue(0.0);
            anim->setEndValue(1.0);
            anim->setEasingCurve(QEasingCurve::InOutQuad);
            connect(anim, &QPropertyAnimation::finished, [prev]() {
                static_cast<QGraphicsOpacityEffect*>(prev->graphicsEffect())->setOpacity(0.0);
            });
            anim->start(QAbstractAnimation::DeleteWhenStopped);
        });
    });
}

void PhotoFrame::prevSlide() {
    if (m_isSleeping) return;
    m_idx = (m_idx - 2 + m_playlist.size()) % m_playlist.size();
    nextSlide();
}

void PhotoFrame::mousePressEvent(QMouseEvent* e) {
    if (m_currentPage == 0 && !m_isSleeping) {
        if (e->button() == Qt::LeftButton) {
            nextSlide();
            m_controlBar->raise();
        }
    }
    e->accept();
}

void PhotoFrame::keyPressEvent(QKeyEvent* e) {
    if (e->key() == Qt::Key_Escape || e->key() == Qt::Key_S) {
        if (m_isSleeping) setSleepMode(false);
        switchToPage(m_currentPage == 1 ? 0 : 1);
    }
    if (m_isSleeping) return;
    if (e->key() == Qt::Key_Right) nextSlide();
    if (e->key() == Qt::Key_Left) prevSlide();
    if (e->key() == Qt::Key_Space) {
        if (m_timer->isActive()) m_timer->stop(); else m_timer->start();
    }
    if (e->key() == Qt::Key_V) switchToPage(m_currentPage == 0 ? -1 : 0);
    if (e->key() == Qt::Key_Q) qApp->quit();
    if (e->key() == Qt::Key_T) { refreshTasks(); switchToPage(m_currentPage == 2 ? 0 : 2); }
    if (e->key() == Qt::Key_C) switchToPage(m_currentPage == 3 ? 0 : 3);
}

void PhotoFrame::startRtspStream() {
    if (!m_config.useRtsp || m_config.rtspUrl.isEmpty()) return;
    m_timer->stop();
    m_showingRtsp = true;
    m_rtspViewer->setUrl(m_config.rtspUrl);
    m_rtspViewer->show();
    m_rtspViewer->raise();
    m_controlBar->raise();
    m_rtspViewer->play();
}

void PhotoFrame::stopRtspStream() {
    m_showingRtsp = false;
    m_rtspViewer->stop();
    m_rtspViewer->hide();
}

void PhotoFrame::resizeEvent(QResizeEvent* e) {
    QMainWindow::resizeEvent(e);
    if (m_isSleeping) m_sleepScreen->resize(size());
    m_controlBar->raise();
}

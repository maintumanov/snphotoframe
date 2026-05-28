#include "photoframe.h"
#include "imagedisplay.h"
#include "playlistmanager.h"

#include <QtWidgets>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QProcess>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QDirIterator>
#include <QDateTime>
#include <QImageReader>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <algorithm>
#include <random>

PhotoFrame::PhotoFrame() {
    setWindowState(Qt::WindowFullScreen);
    setWindowFlags(Qt::FramelessWindowHint);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    m_config.load();
    setupUi();

    m_clockTimer = new QTimer(this);
    connect(m_clockTimer, &QTimer::timeout, this, &PhotoFrame::updateClock);
    m_clockTimer->start(1000);

    m_uiTimer = new QTimer(this);
    connect(m_uiTimer, &QTimer::timeout, [this]() { m_setBtn->hide(); });

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &PhotoFrame::nextSlide);

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

void PhotoFrame::setupUi() {
    QWidget* central = new QWidget(this);
    central->setMouseTracking(true);
    setCentralWidget(central);
    central->setStyleSheet("background: black;");

    m_stack = new QStackedLayout(central);
    m_stack->setStackingMode(QStackedLayout::StackAll);

    m_viewA = new ImageDisplay;
    m_viewB = new ImageDisplay;
    m_stack->addWidget(m_viewA);
    m_stack->addWidget(m_viewB);

    for (auto* v : {m_viewA, m_viewB}) {
        auto* eff = new QGraphicsOpacityEffect(v);
        eff->setOpacity(v == m_viewA ? 1.0 : 0.0);
        v->setGraphicsEffect(eff);
    }

    m_clock = new QLabel(central);
    m_date = new QLabel(central);
    QString style = "color: white; background: rgba(0,0,0,80); padding: 10px; border-radius: 8px;";
    m_clock->setStyleSheet(style + "font-size: 45pt; font-weight: bold;");
    m_date->setStyleSheet(style + "font-size: 18pt;");

    m_photoDate = new QLabel(central);
    m_photoDate->setStyleSheet("color: rgba(255,255,255,180); font-size: 14pt; background: rgba(0,0,0,50); padding: 5px;");
    m_photoDate->setAlignment(Qt::AlignRight);

    m_setBtn = new QPushButton("⚙", central);
    m_setBtn->setFixedSize(70, 70);
    m_setBtn->setStyleSheet("QPushButton{background: rgba(80,80,80,150); color: white; border-radius: 35px; font-size: 25pt;}");
    m_setBtn->hide();
    connect(m_setBtn, &QPushButton::clicked, this, &PhotoFrame::showSettings);

    // Инициализация виджета "Ночного сна" (полностью черный экран)
    m_sleepScreen = new QWidget(central);
    m_sleepScreen->setStyleSheet("background-color: black;");
    m_sleepScreen->hide(); // Скрыт по умолчанию

    updateClock();
}

void PhotoFrame::raiseOverlays() {
    if (m_isSleeping)
        return; // Во сне интерфейс не поднимаем
    m_clock->raise();
    m_date->raise();
    m_photoDate->raise();
    if (!m_setBtn->isHidden())
        m_setBtn->raise();
}

void PhotoFrame::checkSchedule() {
    if (!m_config.useSchedule)
        return;

    QTime now = QTime::currentTime();
    bool shouldBeAwake = false;

    // Проверка с учетом перехода через полночь (например, спим с 23:00 до 07:00)
    if (m_config.wakeTime <= m_config.sleepTime) {
        shouldBeAwake = (now >= m_config.wakeTime && now < m_config.sleepTime);
    } else {
        shouldBeAwake = (now >= m_config.wakeTime || now < m_config.sleepTime);
    }

    if (shouldBeAwake && m_isSleeping) {
        setSleepMode(false);
    } else if (!shouldBeAwake && !m_isSleeping) {
        setSleepMode(true);
    }
}

void PhotoFrame::setSleepMode(bool sleep) {
    m_isSleeping = sleep;
    if (sleep) {
        m_timer->stop();
        m_sleepScreen->resize(size());
        m_sleepScreen->raise();
        m_sleepScreen->show();

        // Для Raspberry Pi можно раскомментировать аппаратное отключение HDMI:
        // QProcess::execute("vcgencmd", {"display_power", "0"});
    } else {
        m_sleepScreen->hide();
        // Включение HDMI на Raspberry Pi:
        // QProcess::execute("vcgencmd", {"display_power", "1"});

        if (!m_playlist.isEmpty()) {
            m_timer->start(m_config.interval);
            nextSlide();
        }
    }
}

void PhotoFrame::updateClock() {
    // Вызывается каждую секунду
    m_clock->setText(QTime::currentTime().toString("HH:mm"));
    m_date->setText(QDate::currentDate().toString("dd MMMM yyyy"));
    m_clock->adjustSize();
    m_date->adjustSize();

    m_clock->move(width() - m_clock->width() - 30, 30);
    m_date->move(width() - m_date->width() - 30, m_clock->y() + m_clock->height() + 5);

    m_photoDate->adjustSize();
    m_photoDate->move(width() - m_photoDate->width() - 30, height() - m_photoDate->height() - 30);

    m_setBtn->move(30, 30);

    // Ресайз черного экрана (если изменили разрешение на лету)
    if (m_isSleeping)
        m_sleepScreen->resize(size());

    checkSchedule();
}

void PhotoFrame::connectAndScan() {
    if (m_config.server.isEmpty()) {
        showSettings();
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
        while (it.hasNext())
            files << it.next();
        QMetaObject::invokeMethod(this, "onScanFinished", Q_ARG(QStringList, files));
    });
}

void PhotoFrame::onScanFinished(const QStringList& list) {
    if (list.isEmpty()) {
        showSettings();
        return;
    }
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
    if (m_playlist.isEmpty() || m_isSleeping)
        return; // Защита от смены кадра во сне
    QString path = m_playlist[m_idx];
    m_idx = (m_idx + 1) % m_playlist.size();

    QFileInfo info(path);
    QString dateStr = "Снято: " + info.lastModified().toString("dd.MM.yyyy HH:mm");

    QtConcurrent::run([this, path, dateStr]() {
        QImageReader reader(path);
        reader.setAutoTransform(true);
        QImage img = reader.read();
        if (img.isNull()) {
            QMetaObject::invokeMethod(this, "nextSlide");
            return;
        }

        QMetaObject::invokeMethod(this, [this, img, dateStr]() {
            auto* effA = static_cast<QGraphicsOpacityEffect*>(m_viewA->graphicsEffect());
            ImageDisplay* next = (effA && effA->opacity() > 0.5) ? m_viewB : m_viewA;
            ImageDisplay* prev = (next == m_viewA) ? m_viewB : m_viewA;

            next->setImage(img);
            m_photoDate->setText(dateStr);

            next->raise();
            raiseOverlays();

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
    if (m_isSleeping)
        return;
    m_idx = (m_idx - 2 + m_playlist.size()) % m_playlist.size();
    nextSlide();
}

void PhotoFrame::mouseMoveEvent(QMouseEvent* e) {
    // В спящем режиме движение мыши/касание может "будить" экран, если расписание выключено
    if (!m_isSleeping) {
        m_setBtn->show();
        raiseOverlays();
        m_uiTimer->start(4000);
    }
    e->accept();
}

void PhotoFrame::mousePressEvent(QMouseEvent* e) {
    m_startPos = e->pos();
    this->setFocus();

    // Если нажат экран в спящем режиме, можно добавить логику временного пробуждения
    // (например, включить экран на 1 минуту, затем снова уснуть)

    e->accept();
}

void PhotoFrame::mouseReleaseEvent(QMouseEvent* e) {
    if (m_isSleeping)
        return;
    int dx = e->pos().x() - m_startPos.x();
    if (qAbs(dx) > 60) {
        if (dx > 0)
            prevSlide();
        else
            nextSlide();
    } else {
        if (m_timer->isActive())
            m_timer->stop();
        else
            m_timer->start();
    }
    e->accept();
}

void PhotoFrame::keyPressEvent(QKeyEvent* e) {
    if (e->key() == Qt::Key_Escape || e->key() == Qt::Key_S) {
        // Позволяем зайти в настройки даже если экран "спит"
        if (m_isSleeping)
            setSleepMode(false);
        showSettings();
    }
    if (m_isSleeping)
        return;

    if (e->key() == Qt::Key_Right)
        nextSlide();
    if (e->key() == Qt::Key_Left)
        prevSlide();
    if (e->key() == Qt::Key_Space)
        mouseReleaseEvent(nullptr);
    if (e->key() == Qt::Key_Q)
        qApp->quit();
}

void PhotoFrame::showSettings() {
    m_timer->stop();
    QDialog d(this);
    d.setWindowTitle("Настройки");
    QFormLayout f(&d);

    // Вкладка Сеть / Основные
    QLineEdit *srv = new QLineEdit(m_config.server), *shr = new QLineEdit(m_config.share);
    QLineEdit *usr = new QLineEdit(m_config.user), *psw = new QLineEdit(m_config.pass);
    psw->setEchoMode(QLineEdit::Password);
    QSpinBox *intv = new QSpinBox;
    intv->setRange(2, 3600);
    intv->setValue(m_config.interval / 1000);
    QCheckBox *shuf = new QCheckBox("Случайный порядок");
    shuf->setChecked(m_config.shuffle);

    // Вкладка Расписание
    QCheckBox *useSch = new QCheckBox("Включить спящий режим");
    useSch->setChecked(m_config.useSchedule);
    QTimeEdit *wake = new QTimeEdit(m_config.wakeTime);
    wake->setDisplayFormat("HH:mm");
    QTimeEdit *sleep = new QTimeEdit(m_config.sleepTime);
    sleep->setDisplayFormat("HH:mm");

    f.addRow("Сервер/IP:", srv);
    f.addRow("Папка:", shr);
    f.addRow("Логин:", usr);
    f.addRow("Пароль:", psw);
    f.addRow("Интервал (сек):", intv);
    f.addRow(shuf);
    f.addRow(new QLabel("<b>Расписание</b>"));
    f.addRow(useSch);
    f.addRow("Время пробуждения:", wake);
    f.addRow("Время сна:", sleep);

    QPushButton *save = new QPushButton("Сохранить");
    QPushButton *exit = new QPushButton("Выход");
    f.addRow(save);
    f.addRow(exit);

    connect(exit, &QPushButton::clicked, qApp, &QCoreApplication::quit);
    connect(save, &QPushButton::clicked, [&]() {
        m_config.server = srv->text();
        m_config.share = shr->text();
        m_config.user = usr->text();
        m_config.pass = psw->text();
        m_config.interval = intv->value() * 1000;
        m_config.shuffle = shuf->isChecked();

        m_config.useSchedule = useSch->isChecked();
        m_config.wakeTime = wake->time();
        m_config.sleepTime = sleep->time();

        m_config.save();

        PlaylistManager::clear();
        d.accept();
        connectAndScan();
    });
    d.exec();

    // Если после закрытия настроек время работы, то стартуем
    checkSchedule();
    if (!m_isSleeping && !m_playlist.isEmpty())
        m_timer->start();
}



#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QTime>
#include <QSettings>

struct SmbConfig {
    QString server, share, user, pass, smbVers = "3.0";
    bool useGuest = true, shuffle = true;
    int interval = 5000;

    // Параметры расписания
    bool useSchedule = false;
    QTime wakeTime = QTime(7, 0);   // Время включения по умолчанию
    QTime sleepTime = QTime(23, 0); // Время выключения по умолчанию

    void load(const QString& fileName = "photoframe.ini");
    void save(const QString& fileName = "photoframe.ini") const;
};

#endif // CONFIG_H

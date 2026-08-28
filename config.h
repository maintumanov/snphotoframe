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
    
    // RTSP камера
    bool useRtsp = false;
    QString rtspUrl;

    // SignalNet интеграция
    bool useSignalNet = false;
    bool useUdp = false;
    QString signalNetServer;
    quint16 signalNetPort = 8888;
    QString signalNetLogin;
    QString signalNetPass;
    quint16 signalNetUdpLocalPort = 29545;
    QString signalNetUdpKey = "signalnet";

    // Камера по SignalNet
    int cameraDuration = 30;

    void load(const QString& fileName = "photoframe.ini");
    void save(const QString& fileName = "photoframe.ini") const;
};

#endif // CONFIG_H

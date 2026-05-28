#include "config.h"

void SmbConfig::load(const QString& fileName) {
    QSettings s(fileName, QSettings::IniFormat);
    server = s.value("smb/server").toString();
    share = s.value("smb/share").toString();
    user = s.value("smb/user").toString();
    pass = s.value("smb/pass").toString();
    interval = s.value("ui/interval", 5000).toInt();
    shuffle = s.value("ui/shuffle", true).toBool();

    useSchedule = s.value("schedule/enable", false).toBool();
    wakeTime = s.value("schedule/wake", QTime(7, 0)).toTime();
    sleepTime = s.value("schedule/sleep", QTime(23, 0)).toTime();
    
    useRtsp = s.value("rtsp/enable", false).toBool();
    rtspUrl = s.value("rtsp/url").toString();
}

void SmbConfig::save(const QString& fileName) const {
    QSettings s(fileName, QSettings::IniFormat);
    s.setValue("smb/server", server);
    s.setValue("smb/share", share);
    s.setValue("smb/user", user);
    s.setValue("smb/pass", pass);
    s.setValue("ui/interval", interval);
    s.setValue("ui/shuffle", shuffle);

    s.setValue("schedule/enable", useSchedule);
    s.setValue("schedule/wake", wakeTime);
    s.setValue("schedule/sleep", sleepTime);
    
    s.setValue("rtsp/enable", useRtsp);
    s.setValue("rtsp/url", rtspUrl);
}

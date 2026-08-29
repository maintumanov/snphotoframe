#include "config.h"

void SmbConfig::load(const QString& fileName) {
    QSettings s(fileName, QSettings::IniFormat);
    server = s.value("smb/server").toString();
    share = s.value("smb/share").toString();
    user = s.value("smb/user").toString();
    pass = s.value("smb/pass").toString();
    useGuest = s.value("smb/guest", true).toBool();
    smbVers = s.value("smb/vers", "3.0").toString();
    interval = s.value("ui/interval", 5000).toInt();
    shuffle = s.value("ui/shuffle", true).toBool();

    useSchedule = s.value("schedule/enable", false).toBool();
    wakeTime = s.value("schedule/wake", QTime(7, 0)).toTime();
    sleepTime = s.value("schedule/sleep", QTime(23, 0)).toTime();

    useRtsp = s.value("rtsp/enable", false).toBool();
    rtspUrl = s.value("rtsp/url").toString();

    useSignalNet = s.value("signalnet/enable", false).toBool();
    useUdp = s.value("signalnet/useudp", false).toBool();
    signalNetServer = s.value("signalnet/server").toString();
    signalNetPort = static_cast<quint16>(s.value("signalnet/port", 8888).toUInt());
    signalNetLogin = s.value("signalnet/login").toString();
    signalNetPass = s.value("signalnet/pass").toString();
    signalNetDeviceAddress = static_cast<quint16>(s.value("signalnet/deviceaddress", 3999).toUInt());
    signalNetUdpLocalPort = static_cast<quint16>(s.value("signalnet/udplocalport", 29545).toUInt());
    signalNetUdpKey = s.value("signalnet/udpkey", "signalnet").toString();
    cameraDuration = s.value("signalnet/cameraduration", 30).toInt();

    useRtsp2 = s.value("rtsp2/enable", false).toBool();
    rtspUrl2 = s.value("rtsp2/url").toString();
    camera2Duration = s.value("rtsp2/cameraduration", 30).toInt();

    useRtsp3 = s.value("rtsp3/enable", false).toBool();
    rtspUrl3 = s.value("rtsp3/url").toString();
    camera3Duration = s.value("rtsp3/cameraduration", 30).toInt();
}

void SmbConfig::save(const QString& fileName) const {
    QSettings s(fileName, QSettings::IniFormat);
    s.setValue("smb/server", server);
    s.setValue("smb/share", share);
    s.setValue("smb/user", user);
    s.setValue("smb/pass", pass);
    s.setValue("smb/guest", useGuest);
    s.setValue("smb/vers", smbVers);
    s.setValue("ui/interval", interval);
    s.setValue("ui/shuffle", shuffle);

    s.setValue("schedule/enable", useSchedule);
    s.setValue("schedule/wake", wakeTime);
    s.setValue("schedule/sleep", sleepTime);

    s.setValue("rtsp/enable", useRtsp);
    s.setValue("rtsp/url", rtspUrl);

    s.setValue("signalnet/enable", useSignalNet);
    s.setValue("signalnet/useudp", useUdp);
    s.setValue("signalnet/server", signalNetServer);
    s.setValue("signalnet/port", signalNetPort);
    s.setValue("signalnet/login", signalNetLogin);
    s.setValue("signalnet/pass", signalNetPass);
    s.setValue("signalnet/deviceaddress", signalNetDeviceAddress);
    s.setValue("signalnet/udplocalport", signalNetUdpLocalPort);
    s.setValue("signalnet/udpkey", signalNetUdpKey);
    s.setValue("signalnet/cameraduration", cameraDuration);

    s.setValue("rtsp2/enable", useRtsp2);
    s.setValue("rtsp2/url", rtspUrl2);
    s.setValue("rtsp2/cameraduration", camera2Duration);

    s.setValue("rtsp3/enable", useRtsp3);
    s.setValue("rtsp3/url", rtspUrl3);
    s.setValue("rtsp3/cameraduration", camera3Duration);
}

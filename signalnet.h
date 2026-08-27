#ifndef SIGNALNET_H
#define SIGNALNET_H

#include <QObject>
#include <QTimer>
#include <QString>

// QSN library types (used in eventNumInput signatures)
#include "qsnshapes.h"

class QsnInterface;
class QsnTCPclient;
class QsnUDPclient;

// Device parameters (from DevicePattern_1002.xml)
// -- основные --
#define MemoryBlocks 64
#define DeviceType 1002
#define FirmwareVersionMajor 1
#define FirmwareVersionMinor1 0
#define FirmwareVersionMinor2 0
#define FirmwareVersionMinor3 0

#define IO_count 126

// -- cвойства --
#define sundry_darkIcon 2,0

// -- выхода --
#define output_temperatureSetting 0
#define output_pulse 1
#define output_action1 2
#define output_action2 3

// -- входа --
#define input_statusAbsence 0
#define input_statusNoAbsence 1
#define input_alert 2
#define input_alarm 3
#define input_bell 4
#define input_temperature 5
#define input_pause 6
#define input_playstop 7
#define input_next 8
#define input_previous 9
#define input_right 10
#define input_left 11

// -- отладка --


// Output command indices (use #define values)
#define SnOutputTemperatureSetting output_temperatureSetting
#define SnOutputPulse output_pulse
#define SnOutputAction1 output_action1
#define SnOutputAction2 output_action2

class SignalNet : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(qreal temperature READ temperature NOTIFY temperatureChanged)
    Q_PROPERTY(qreal thermostatSetting READ thermostatSetting NOTIFY thermostatSettingChanged)
    Q_PROPERTY(QString lastAlert READ lastAlert NOTIFY lastAlertChanged)
    Q_PROPERTY(int alertSeverity READ alertSeverity NOTIFY alertSeverityChanged)
    Q_PROPERTY(bool absenceMode READ absenceMode NOTIFY absenceModeChanged)
    Q_PROPERTY(bool useUdp READ useUdp NOTIFY useUdpChanged)

public:
    explicit SignalNet(QObject *parent = nullptr);
    ~SignalNet();

    bool isConnected() const;
    qreal temperature() const;
    qreal thermostatSetting() const;
    QString lastAlert() const;
    int alertSeverity() const;
    bool absenceMode() const;
    bool useUdp() const;

    Q_INVOKABLE void connectToServer(const QString &address, quint16 port,
                                     const QString &login, const QString &password);
    Q_INVOKABLE void connectToServerUDP(const QString &address, quint16 serverPort,
                                        quint16 localPort, const QString &key);
    Q_INVOKABLE void disconnectFromServer();
    Q_INVOKABLE void clearAlert();
    Q_INVOKABLE void setTransport(bool useUdp);

signals:
    void connectedChanged();
    void temperatureChanged();
    void thermostatSettingChanged();
    void lastAlertChanged();
    void absenceModeChanged();
    void useUdpChanged();

    // Media control signals for photo frame
    void mediaNext();
    void mediaPrevious();
    void mediaPlayPause();
    void mediaStop();

    // Navigation signals
    void navRight();
    void navLeft();

    // System signals
    void bellPressed();
    void alertReceived(const QString &type, int severity);
    void poweroffRequested();

    // Connection status
    void connectionError(const QString &msg);
    void alertSeverityChanged(int severity);

private slots:
    void onEventNumInput(int numInput, QByteArray data);
    void onEventConnect();
    void onEventDisconnect();
    void onSnBUSInput(QSNContainer container, QObject *sender);
    void checkReconnect();

private:
    QsnInterface *m_interface;
    QsnTCPclient *m_tcpClient;
    QsnUDPclient *m_udpClient;
    QTimer *m_pulseTimer;
    QTimer *m_reconnectTimer;

    bool m_connected;
    bool m_userDisconnected;
    bool m_useUdp;
    bool m_lastUseUdp = false;
    QString m_lastConnectAddress;
    quint16 m_lastConnectPort = 0;
    QString m_lastConnectLogin;
    QString m_lastConnectPass;
    quint16 m_deviceAddress;
    qreal m_temperature;
    qreal m_thermostatSetting;
    QString m_lastAlert;
    int m_alertSeverity;
    bool m_absenceMode;

    void setupTransport();
};

#endif // SIGNALNET_H

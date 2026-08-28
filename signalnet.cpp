#include "signalnet.h"
#include "qsninterface.h"
#include "qsntcpclient.h"
#include "qsnudpclient.h"
#include "qsnshapes.h"
#include <QDebug>
#include <QCoreApplication>

SignalNet::SignalNet(QObject *parent)
    : QObject(parent)
    , m_interface(new QsnInterface(this))
    , m_tcpClient(new QsnTCPclient(this))
    , m_udpClient(new QsnUDPclient(this))
    , m_pulseTimer(new QTimer(this))
    , m_connected(false)
    , m_userDisconnected(false)
    , m_useUdp(false)
    , m_deviceAddress(3999)
    , m_temperature(0)
    , m_thermostatSetting(0)
    , m_alertSeverity(0)
    , m_absenceMode(false)
{
    // Wire up the QSN bus — both clients feed into the interface,
    // and the interface broadcasts back to both (same as snpcagent mainwindow.cpp)
    connect(m_tcpClient, SIGNAL(snBUSOutput(QSNContainer,QObject*)),
            m_interface, SLOT(snBUSInput(QSNContainer,QObject*)));
    connect(m_udpClient, SIGNAL(snBUSOutput(QSNContainer,QObject*)),
            m_interface, SLOT(snBUSInput(QSNContainer,QObject*)));
    connect(m_interface, SIGNAL(snBUSOutput(QSNContainer,QObject*)),
            m_tcpClient, SLOT(snBUSInput(QSNContainer,QObject*)));
    connect(m_interface, SIGNAL(snBUSOutput(QSNContainer,QObject*)),
            m_udpClient, SLOT(snBUSInput(QSNContainer,QObject*)));

    // Application listens to interface events
    connect(m_interface, &QsnInterface::eventConnect,
            this, &SignalNet::onEventConnect);
    connect(m_interface, &QsnInterface::eventDisconnect,
            this, &SignalNet::onEventDisconnect);
    connect(m_interface, &QsnInterface::eventNumInput,
            this, &SignalNet::onEventNumInput);

    // Pulse request handler (Command=2 from QsnInterface::secondOut timer)
    connect(m_interface, SIGNAL(snBUSOutput(QSNContainer,QObject*)),
            this, SLOT(onSnBUSInput(QSNContainer,QObject*)));

    // Configure interface for digital photo frame device
    m_interface->actionSetAddress(m_deviceAddress);
    m_interface->setMemorySize(MemoryBlocks);
    m_interface->setIOcount(IO_count);
    m_interface->setDeviceTypeIndex(DeviceType);
    m_interface->setMemoryFileName(
        QSNHomePath("photoframe").absoluteFilePath("digitalphotoframe.idm"));

    // Reconnect timer (like snpcagent connectTimer)
    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setSingleShot(true);
    connect(m_reconnectTimer, &QTimer::timeout, this, &SignalNet::checkReconnect);

    // Default UDP source port
    m_udpClient->setSourcePort(29545);

    // Check temperature staleness every minute
    QTimer *staleTimer = new QTimer(this);
    connect(staleTimer, &QTimer::timeout, this, [this]() {
        if (m_lastTemperatureTime.isValid() && !isTemperatureValid()) {
            m_lastTemperatureTime = QDateTime(); // invalidate
            emit temperatureValidChanged();
        }
    });
    staleTimer->start(60000);
}

SignalNet::~SignalNet()
{
    disconnectFromServer();
}

bool SignalNet::isConnected() const { return m_connected; }
qreal SignalNet::temperature() const { return m_temperature; }
qreal SignalNet::thermostatSetting() const { return m_thermostatSetting; }
QString SignalNet::lastAlert() const { return m_lastAlert; }
int SignalNet::alertSeverity() const { return m_alertSeverity; }
bool SignalNet::absenceMode() const { return m_absenceMode; }
bool SignalNet::useUdp() const { return m_useUdp; }
bool SignalNet::isTemperatureValid() const {
    return m_lastTemperatureTime.isValid() &&
           m_lastTemperatureTime.secsTo(QDateTime::currentDateTime()) < 4 * 3600;
}

void SignalNet::setupTransport()
{
    // Enable the selected transport, disable the other (same as snpcagent settingsApply)
    m_tcpClient->setEnabled(!m_useUdp);
    m_udpClient->setEnabled(m_useUdp);
}

void SignalNet::setTransport(bool useUdp)
{
    if (m_useUdp != useUdp) {
        m_useUdp = useUdp;
        emit useUdpChanged();
    }
}

void SignalNet::connectToServer(const QString &address, quint16 port,
                                 const QString &login, const QString &password)
{
    qInfo() << "SignalNet: connecting TCP to" << address << ":" << port;

    m_lastConnectAddress = address;
    m_lastConnectPort = port;
    m_lastConnectLogin = login;
    m_lastConnectPass = password;

    m_tcpClient->setAddress(address);
    m_tcpClient->setPort(port);
    m_tcpClient->setLoginAndPassword(login, password);
    m_tcpClient->setDeviceName(QCoreApplication::applicationName());

    m_useUdp = false;
    m_lastUseUdp = false;
    m_userDisconnected = false;
    emit useUdpChanged();
    setupTransport();
    m_reconnectTimer->stop();
    m_interface->actionConnect();
}

void SignalNet::connectToServerUDP(const QString &address, quint16 serverPort,
                                    quint16 localPort, const QString &key)
{
    qInfo() << "SignalNet: connecting UDP to" << address << ":" << serverPort;

    m_lastConnectAddress = address;
    m_lastConnectPort = serverPort;

    m_udpClient->setAddress(address);
    m_udpClient->setServerPort(serverPort);
    m_udpClient->setSourcePort(localPort);
    m_udpClient->setKey(key);
    m_udpClient->setDeviceName(QCoreApplication::applicationName());

    m_useUdp = true;
    m_lastUseUdp = true;
    m_userDisconnected = false;
    emit useUdpChanged();
    setupTransport();
    m_reconnectTimer->stop();
    m_interface->actionConnect();
}

void SignalNet::disconnectFromServer()
{
    m_userDisconnected = true;
    m_reconnectTimer->stop();
    m_pulseTimer->stop();
    m_interface->actionDisconnect();
    if (m_connected) {
        m_connected = false;
        emit connectedChanged();
    }
}

void SignalNet::clearAlert()
{
    if (!m_lastAlert.isEmpty()) {
        m_lastAlert.clear();
        m_alertSeverity = 0;
        emit lastAlertChanged();
        emit alertSeverityChanged(0);
    }
}

void SignalNet::onEventConnect()
{
    qInfo() << "SignalNet: connected and authorized via"
            << (m_useUdp ? "UDP" : "TCP");
    m_connected = true;
    m_reconnectTimer->stop();
    emit connectedChanged();
    m_pulseTimer->start(10000);

    // Request thermostat setting from controller (same as snpcagent thermostatRequest)
    QByteArray data;
    data.append(static_cast<char>(9));
    QSNTemperatureToRAW(&data, 1, -110);
    m_interface->actionNumOutput(SnOutputTemperatureSetting, data);
}

void SignalNet::onEventDisconnect()
{
    qInfo() << "SignalNet: disconnected";
    m_pulseTimer->stop();
    if (m_connected) {
        m_connected = false;
        emit connectedChanged();
    }
    // Auto-reconnect if not user-initiated disconnect (like snpcagent connectTimer)
    if (!m_userDisconnected && !m_lastConnectAddress.isEmpty()) {
        qInfo() << "SignalNet: will reconnect in 5s";
        m_reconnectTimer->start(5000);
    }
}

void SignalNet::checkReconnect()
{
    if (m_connected || m_userDisconnected || m_lastConnectAddress.isEmpty())
        return;

    qInfo() << "SignalNet: attempting reconnect";
    // Use the transport that was active at last connect, not the current setting
    if (m_lastUseUdp) {
        m_udpClient->setAddress(m_lastConnectAddress);
        m_udpClient->setServerPort(m_lastConnectPort);
        m_udpClient->setDeviceName(QCoreApplication::applicationName());
        m_useUdp = true;
    } else {
        m_tcpClient->setAddress(m_lastConnectAddress);
        m_tcpClient->setPort(m_lastConnectPort);
        m_tcpClient->setLoginAndPassword(m_lastConnectLogin, m_lastConnectPass);
        m_tcpClient->setDeviceName(QCoreApplication::applicationName());
        m_useUdp = false;
    }
    emit useUdpChanged();
    setupTransport();
    m_interface->actionConnect();
}

void SignalNet::onSnBUSInput(QSNContainer container, QObject *sender)
{
    Q_UNUSED(sender);
    if (container.role != QSNContainer::service) return;
    if (container.Command == 2) {
        // Pulse request from QsnInterface::secondOut — respond
        if (m_connected) {
            m_interface->actionNumOutput(SnOutputPulse);
        }
    }
}

void SignalNet::onEventNumInput(int numInput, QByteArray data)
{
    switch (numInput) {
    case input_temperature: {
        qreal temp = QSNRAWtoTemperature(&data, 1);
        bool wasValid = isTemperatureValid();
        m_lastTemperatureTime = QDateTime::currentDateTime();
        if (m_temperature != temp) {
            m_temperature = temp;
            emit temperatureChanged();
            qInfo() << "SignalNet: temperature =" << temp << "\u00b0C";
        }
        if (!wasValid) emit temperatureValidChanged();
        break;
    }
    case input_alert: {
        if (data.size() >= 1) {
            QSNNotification notif = QSNRAWtoNotification(&data, 1);
            QString alertText = QSNAlertTypeLabel(notif.notificationType);
            if (!alertText.isEmpty()) {
                m_lastAlert = alertText;
                m_alertSeverity = (notif.notificationType % 2 == 1) ? 1 : 0;
                emit lastAlertChanged();
                emit alertSeverityChanged(m_alertSeverity);
                emit alertReceived(m_lastAlert, m_alertSeverity);
                qInfo() << "SignalNet: alert:" << m_lastAlert;
            }
        }
        break;
    }
    case input_alarm: {
        if (data.size() >= 1) {
            QSNNotification notif = QSNRAWtoNotification(&data, 1);
            QString alarmText = QSNSecurityAlertTypeLabel(notif.notificationType);
            if (!alarmText.isEmpty()) {
                m_lastAlert = alarmText;
                m_alertSeverity = (notif.notificationType == 12) ? 2 : 0;
                emit lastAlertChanged();
                emit alertSeverityChanged(m_alertSeverity);
                emit alertReceived(m_lastAlert, m_alertSeverity);
            }
        }
        break;
    }
    case input_bell: {
        m_lastAlert = QString::fromUtf8("\u0417\u0432\u043e\u043d\u043e\u043a");
        m_alertSeverity = 0;
        qInfo() << "SignalNet:" << m_lastAlert;
        emit lastAlertChanged();
        emit alertSeverityChanged(0);
        emit alertReceived(m_lastAlert, 0);
        emit bellPressed(m_lastAlert);
        break;
    }
    case input_playstop: {
        qInfo() << "SignalNet: play/stop";
        emit mediaPlayPause();
        break;
    }
    case input_pause: {
        qInfo() << "SignalNet: pause";
        emit mediaPlayPause();
        break;
    }
    case input_next: {
        qInfo() << "SignalNet: next";
        emit mediaNext();
        break;
    }
    case input_previous: {
        qInfo() << "SignalNet: previous";
        emit mediaPrevious();
        break;
    }
    case input_right: {
        qInfo() << "SignalNet: right";
        emit navRight();
        break;
    }
    case input_left: {
        qInfo() << "SignalNet: left";
        emit navLeft();
        break;
    }
    case input_camera: {
        qInfo() << "SignalNet: camera on";
        emit cameraOn();
        break;
    }
    case input_statusAbsence: {
        if (!m_absenceMode) {
            m_absenceMode = true;
            emit absenceModeChanged();
            qInfo() << "SignalNet: absence mode ON";
        }
        break;
    }
    case input_statusNoAbsence: {
        if (m_absenceMode) {
            m_absenceMode = false;
            emit absenceModeChanged();
            qInfo() << "SignalNet: absence mode OFF";
        }
        break;
    }
    default:
        break;
    }
}

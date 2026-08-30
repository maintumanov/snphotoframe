#include "webserver.h"
#include "photoframe.h"
#include "signalnet.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkInterface>
#include <QFile>
#include <QDir>
#include <QCoreApplication>

WebServer::WebServer(PhotoFrameBackend *backend, QObject *parent)
    : QObject(parent), m_server(new QTcpServer(this)), m_backend(backend), m_port(8080)
{
    connect(m_server, &QTcpServer::newConnection, this, &WebServer::onNewConnection);
}

bool WebServer::start(quint16 port)
{
    m_port = port;
    if (m_server->isListening()) m_server->close();
    if (!m_server->listen(QHostAddress::Any, port)) {
        qWarning() << "WebServer: failed to listen on port" << port;
        return false;
    }
    qInfo() << "WebServer: listening on port" << port;
    return true;
}

void WebServer::stop()
{
    m_server->close();
}

void WebServer::onNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket *socket = m_server->nextPendingConnection();
        connect(socket, &QTcpSocket::readyRead, this, &WebServer::onReadyRead);
        connect(socket, &QTcpSocket::disconnected, this, &WebServer::onDisconnected);
    }
}

void WebServer::onReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    m_buffers[socket].append(socket->readAll());
    if (processRequests(socket) > 0)
        socket->disconnectFromHost();
}

int WebServer::processRequests(QTcpSocket *socket)
{
    QByteArray &buf = m_buffers[socket];
    int count = 0;
    for (;;) {
        int headerEnd = buf.indexOf("\r\n\r\n");
        if (headerEnd < 0) break;
        int contentLength = 0;
        const QByteArray headerBlock = buf.left(headerEnd);
        const QList<QByteArray> headerLines = headerBlock.split('\n');
        for (const QByteArray &line : headerLines) {
            int colon = line.indexOf(':');
            if (colon <= 0) continue;
            if (line.left(colon).trimmed().toLower() == "content-length") {
                contentLength = line.mid(colon + 1).trimmed().toInt();
                break;
            }
        }
        const int total = headerEnd + 4 + contentLength;
        if (buf.size() < total) break;
        handleRequest(socket, buf.left(total));
        buf.remove(0, total);
        ++count;
    }
    return count;
}

void WebServer::onDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        m_buffers.remove(socket);
        socket->deleteLater();
    }
}

void WebServer::handleRequest(QTcpSocket *socket, const QByteArray &request)
{
    QByteArray firstLine = request.split('\r')[0].trimmed();
    QList<QByteArray> parts = firstLine.split(' ');
    if (parts.size() < 2) {
        sendResponse(socket, 400, "text/plain", "Bad Request");
        return;
    }

    QByteArray method = parts[0];
    QByteArray pathRaw = parts[1];

    // Parse path and query
    QString path = QString::fromUtf8(pathRaw);
    if (path.contains('?')) path = path.left(path.indexOf('?'));

    // API routes
    if (path == "/api/status" && method == "GET") {
        handleApiGet(socket, "status");
    } else if (path == "/api/settings" && method == "GET") {
        handleApiGet(socket, "settings");
    } else if (path == "/api/settings" && method == "POST") {
        // Extract body (everything after double CRLF)
        int bodyStart = request.indexOf("\r\n\r\n");
        QByteArray body = (bodyStart >= 0) ? request.mid(bodyStart + 4) : "";
        handleApiPost(socket, "settings", body);
    } else if (path == "/api/signalnet/connect" && method == "POST") {
        m_backend->connectSignalNet();
        sendResponse(socket, 200, "application/json", "{\"ok\":true}");
    } else if (path == "/api/signalnet/disconnect" && method == "POST") {
        m_backend->disconnectSignalNet();
        sendResponse(socket, 200, "application/json", "{\"ok\":true}");
    } else if (path == "/api/rtsp" && method == "POST") {
        // RTSP control: {"on":true|false, "camera":1|2|3}
        int bodyStart = request.indexOf("\r\n\r\n");
        QByteArray body = (bodyStart >= 0) ? request.mid(bodyStart + 4) : "";
        handleRtspPost(socket, body);
    } else if (path == "/api/restart" && method == "POST") {
        sendResponse(socket, 200, "application/json", "{\"ok\":true}");
        QTimer::singleShot(500, this, &WebServer::restartRequested);
    } else if (path == "/" || path == "/index.html") {
        // Serve embedded HTML
        QFile htmlFile(QCoreApplication::applicationDirPath() + "/web/index.html");
        if (htmlFile.exists()) {
            htmlFile.open(QIODevice::ReadOnly);
            sendResponse(socket, 200, "text/html; charset=utf-8", htmlFile.readAll());
        } else {
            // Try from resource
            QFile resFile(":/web/index.html");
            if (resFile.exists()) {
                resFile.open(QIODevice::ReadOnly);
                sendResponse(socket, 200, "text/html; charset=utf-8", resFile.readAll());
            } else {
                sendResponse(socket, 200, "text/html; charset=utf-8",
                    "<html><body><h1>DigitalPhotoFrame</h1><p>Web UI not found. Place index.html in web/ folder.</p></body></html>");
            }
        }
    } else {
        sendResponse(socket, 404, "text/plain", "Not Found");
    }
}

void WebServer::sendResponse(QTcpSocket *socket, int code, const QString &contentType, const QByteArray &body)
{
    QByteArray statusText;
    switch(code) {
        case 200: statusText = "OK"; break;
        case 400: statusText = "Bad Request"; break;
        case 404: statusText = "Not Found"; break;
        default: statusText = "Error"; break;
    }

    QByteArray response;
    response.append("HTTP/1.1 " + QByteArray::number(code) + " " + statusText + "\r\n");
    response.append("Content-Type: " + contentType.toUtf8() + "\r\n");
    response.append("Content-Length: " + QByteArray::number(body.size()) + "\r\n");
    response.append("Access-Control-Allow-Origin: *\r\n");
    response.append("Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n");
    response.append("Access-Control-Allow-Headers: Content-Type\r\n");
    response.append("Cache-Control: no-cache\r\n");
    response.append("\r\n");
    response.append(body);

    socket->write(response);
    socket->flush();
    socket->disconnectFromHost();
}

void WebServer::handleApiGet(QTcpSocket *socket, const QString &path)
{
    if (path == "status") {
        sendResponse(socket, 200, "application/json", getJsonStatus().toUtf8());
    } else if (path == "settings") {
        QJsonObject s;
        // SMB
        s["server"] = m_backend->server();
        s["share"] = m_backend->share();
        s["user"] = m_backend->user();
        s["pass"] = m_backend->pass();
        s["useGuest"] = m_backend->useGuest();
        s["smbVers"] = m_backend->smbVers();
        s["interval"] = m_backend->interval();
        s["shuffle"] = m_backend->shuffle();
        s["useActionButtons"] = m_backend->useActionButtons();
        // Schedule
        s["useSchedule"] = m_backend->useSchedule();
        s["wakeTime"] = m_backend->wakeTimeStr();
        s["sleepTime"] = m_backend->sleepTimeStr();
        // RTSP 1
        s["useRtsp"] = m_backend->useRtsp();
        s["rtspUrl"] = m_backend->rtspUrl();
        s["cameraDuration"] = m_backend->cameraDuration();
        // RTSP 2
        s["useRtsp2"] = m_backend->useRtsp2();
        s["rtspUrl2"] = m_backend->rtspUrl2();
        s["camera2Duration"] = m_backend->camera2Duration();
        // RTSP 3
        s["useRtsp3"] = m_backend->useRtsp3();
        s["rtspUrl3"] = m_backend->rtspUrl3();
        s["camera3Duration"] = m_backend->camera3Duration();
        // SignalNet
        s["useSignalNet"] = m_backend->useSignalNet();
        s["signalNetServer"] = m_backend->signalNetServer();
        s["signalNetPort"] = m_backend->signalNetPort();
        s["signalNetLogin"] = m_backend->signalNetLogin();
        s["signalNetPass"] = m_backend->signalNetPass();
        s["signalNetUseUdp"] = m_backend->signalNetUseUdp();
        s["signalNetUdpLocalPort"] = m_backend->signalNetUdpLocalPort();
        s["signalNetUdpKey"] = m_backend->signalNetUdpKey();
        s["signalNetDeviceAddress"] = m_backend->signalNetDeviceAddress();
        sendResponse(socket, 200, "application/json", QJsonDocument(s).toJson());
    } else {
        sendResponse(socket, 404, "text/plain", "Not Found");
    }
}

void WebServer::handleApiPost(QTcpSocket *socket, const QString &path, const QByteArray &body)
{
    if (path == "settings") {
        QJsonDocument doc = QJsonDocument::fromJson(body);
        if (doc.isNull()) {
            sendResponse(socket, 400, "application/json", "{\"error\":\"invalid json\"}");
            return;
        }
        applySettings(doc.object());
        m_backend->saveSettings();
        sendResponse(socket, 200, "application/json", "{\"ok\":true}");
    } else {
        sendResponse(socket, 404, "text/plain", "Not Found");
    }
}

void WebServer::applySettings(const QJsonObject &json)
{
    // SMB
    if (json.contains("server")) m_backend->setServer(json["server"].toString());
    if (json.contains("share")) m_backend->setShare(json["share"].toString());
    if (json.contains("user")) m_backend->setUser(json["user"].toString());
    if (json.contains("pass")) m_backend->setPass(json["pass"].toString());
    if (json.contains("useGuest")) m_backend->setUseGuest(json["useGuest"].toBool());
    if (json.contains("smbVers")) m_backend->setSmbVers(json["smbVers"].toString());
    if (json.contains("interval")) m_backend->setInterval(json["interval"].toInt());
    if (json.contains("shuffle")) m_backend->setShuffle(json["shuffle"].toBool());
    if (json.contains("useActionButtons")) m_backend->setUseActionButtons(json["useActionButtons"].toBool());
    // Schedule
    if (json.contains("useSchedule")) m_backend->setUseSchedule(json["useSchedule"].toBool());
    if (json.contains("wakeTime")) m_backend->setWakeTimeStr(json["wakeTime"].toString());
    if (json.contains("sleepTime")) m_backend->setSleepTimeStr(json["sleepTime"].toString());
    // RTSP 1
    if (json.contains("useRtsp")) m_backend->setUseRtsp(json["useRtsp"].toBool());
    if (json.contains("rtspUrl")) m_backend->setRtspUrl(json["rtspUrl"].toString());
    if (json.contains("cameraDuration")) m_backend->setCameraDuration(json["cameraDuration"].toInt());
    // RTSP 2
    if (json.contains("useRtsp2")) m_backend->setUseRtsp2(json["useRtsp2"].toBool());
    if (json.contains("rtspUrl2")) m_backend->setRtspUrl2(json["rtspUrl2"].toString());
    if (json.contains("camera2Duration")) m_backend->setCamera2Duration(json["camera2Duration"].toInt());
    // RTSP 3
    if (json.contains("useRtsp3")) m_backend->setUseRtsp3(json["useRtsp3"].toBool());
    if (json.contains("rtspUrl3")) m_backend->setRtspUrl3(json["rtspUrl3"].toString());
    if (json.contains("camera3Duration")) m_backend->setCamera3Duration(json["camera3Duration"].toInt());
    // SignalNet
    if (json.contains("useSignalNet")) m_backend->setUseSignalNet(json["useSignalNet"].toBool());
    if (json.contains("signalNetServer")) m_backend->setSignalNetServer(json["signalNetServer"].toString());
    if (json.contains("signalNetPort")) m_backend->setSignalNetPort(json["signalNetPort"].toInt());
    if (json.contains("signalNetLogin")) m_backend->setSignalNetLogin(json["signalNetLogin"].toString());
    if (json.contains("signalNetPass")) m_backend->setSignalNetPass(json["signalNetPass"].toString());
    if (json.contains("signalNetUseUdp")) m_backend->setSignalNetUseUdp(json["signalNetUseUdp"].toBool());
    if (json.contains("signalNetUdpLocalPort")) m_backend->setSignalNetUdpLocalPort(json["signalNetUdpLocalPort"].toInt());
    if (json.contains("signalNetUdpKey")) m_backend->setSignalNetUdpKey(json["signalNetUdpKey"].toString());
    if (json.contains("signalNetDeviceAddress")) m_backend->setSignalNetDeviceAddress(json["signalNetDeviceAddress"].toInt());
}

void WebServer::handleRtspPost(QTcpSocket *socket, const QByteArray &body)
{
    QJsonDocument doc = QJsonDocument::fromJson(body);
    if (doc.isNull()) {
        sendResponse(socket, 400, "application/json", "{\"error\":\"invalid json\"}");
        return;
    }
    QJsonObject o = doc.object();
    const int cam = o.value("camera").toInt(1);
    const bool on = o.value("on").toBool(true);
    if (on) {
        if (cam == 2) m_backend->reconnectRtsp2();
        else if (cam == 3) m_backend->reconnectRtsp3();
        else m_backend->reconnectRtsp();
    } else {
        if (cam == 2) m_backend->stopRtsp2();
        else if (cam == 3) m_backend->stopRtsp3();
        else m_backend->stopRtsp();
    }
    sendResponse(socket, 200, "application/json", "{\"ok\":true}");
}

QString WebServer::getJsonStatus()
{
    QJsonObject s;
    s["connected"] = m_backend->signalNetConnected();
    s["temperature"] = m_backend->signalNetTemperature();
    s["temperatureValid"] = m_backend->signalNetTemperatureValid();
    s["temperatureOut"] = m_backend->signalNetTemperatureOut();
    s["temperatureOutValid"] = m_backend->signalNetTemperatureOutValid();
    s["humidity"] = m_backend->signalNetHumidity();
    s["humidityValid"] = m_backend->signalNetHumidityValid();
    s["co2"] = m_backend->signalNetCo2();
    s["co2Valid"] = m_backend->signalNetCo2Valid();
    s["dust"] = m_backend->signalNetDust();
    s["dustValid"] = m_backend->signalNetDustValid();
    s["alert"] = m_backend->signalNetAlert();
    s["alertSeverity"] = m_backend->signalNetAlertSeverity();
    s["rtspState"] = m_backend->rtspState();
    s["rtsp2State"] = m_backend->rtsp2State();
    s["rtsp3State"] = m_backend->rtsp3State();
    s["currentTime"] = m_backend->currentTime();
    s["currentDate"] = m_backend->currentDate();
    s["sleeping"] = m_backend->property("pageIndex").toInt() == 0;
    s["playlistSize"] = m_backend->tasks().size();
    return QJsonDocument(s).toJson(QJsonDocument::Compact);
}

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QString>

class PhotoFrameBackend;

class WebServer : public QObject
{
    Q_OBJECT
public:
    explicit WebServer(PhotoFrameBackend *backend, QObject *parent = nullptr);
    bool start(quint16 port = 8080);
    void stop();
    bool isRunning() const { return m_server && m_server->isListening(); }
    quint16 port() const { return m_port; }

signals:
    void settingsChanged();
    void restartRequested();

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();

private:
    void handleRequest(QTcpSocket *socket, const QByteArray &request);
    void sendResponse(QTcpSocket *socket, int code, const QString &contentType, const QByteArray &body);
    void handleApiGet(QTcpSocket *socket, const QString &path);
    void handleApiPost(QTcpSocket *socket, const QString &path, const QByteArray &body);
    void handleRtspPost(QTcpSocket *socket, const QByteArray &body);
    QString getJsonStatus();
    void applySettings(const QJsonObject &json);

    QTcpServer *m_server;
    PhotoFrameBackend *m_backend;
    quint16 m_port;
};

#endif // WEBSERVER_H

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include "photoframe.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("DigitalPhotoFrame");

    PhotoFrameBackend backend;
    ImageProvider* imgProvider = new ImageProvider;

    QQmlApplicationEngine engine;
    engine.addImageProvider("current", imgProvider);
    backend.setImageProvider(imgProvider);

    engine.rootContext()->setContextProperty("backend", &backend);

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    QQuickWindow* window = qobject_cast<QQuickWindow*>(engine.rootObjects().first());
    if (window) {
        window->showFullScreen();
    }

    return app.exec();
}

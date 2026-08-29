#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QDebug>
#include "photoframe.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    QGuiApplication app(argc, argv);
    app.setApplicationName("DigitalPhotoFrame");
    app.setApplicationVersion("1.0.0.5");
    qInfo() << "Application started";

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

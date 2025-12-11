#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include<QDir>

#include "UserController.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<UserController>("TelegramCloneQML", 1, 0, "UserController");

    QQmlApplicationEngine engine;

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("TelegramCloneQML", "Main");

    return app.exec();

}
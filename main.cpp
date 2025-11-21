#include "main.h"

#include "modules/AudioVisualizerModel.h"
#include "modules/EggModel.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QRandomGenerator64>
#include <QTimer>
#include <QtGraphs/QLineSeries>
#include <QtGraphs/QValueAxis>


int
main(int argc, char *argv[])
{
    QGuiApplication       app(argc, argv);

    Main::installTheme();

    QQmlApplicationEngine engine;

    qmlRegisterType<EggModel>("Egg", 1, 0, "EggModel");
    qmlRegisterType<EggSensor>("Egg", 1, 0, "EggSensor");

    qmlRegisterType<AudioVisualizerModel>("AudioViz", 1, 0, "AudioVisualizer");

    QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

    engine.load("qrc:/qml/Main.qml");

    return app.exec();
}

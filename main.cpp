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

    QQmlApplicationEngine engine;

    // qmlRegisterType<EggModel>("Egg", 1, 0, "EggModel");

    QLineSeries          *eggSeries = new QLineSeries();
    engine.rootContext()->setContextProperty("eggSeries", eggSeries);

    QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

    engine.load("qrc:/qml/Main.qml");

    QTimer timer;

    timer.setInterval(5);
    timer.setSingleShot(false);

    QObject::connect(&timer, &QTimer::timeout, [eggSeries]() {
        static qreal x = 0;

        eggSeries->append(x, QRandomGenerator64::global()->bounded(0, 5));

        x += 0.1;
    });

    timer.start();

    return app.exec();
}

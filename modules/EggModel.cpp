#include "modules/EggModel.h"

#include "modules/RingBuffer.h"
#include "modules/TimeTrace.h"

#include <QDebug>
#include <QFuture>
#include <QPointF>
#include <QScreen>
#include <QTimer>
#include <QVector>
#include <QWeakPointer>
#include <QtConcurrent/QtConcurrentRun>
#include <QtGraphs/QValueAxis>

struct EggModelPrivate
{
    QVector<QPointF>      batchBuf;

    // این به اندازه تعداد نقاط + اندازه بالایی رزرو میشه
    QVector<QPointF>      points;

    QTimer                uiTimer;

    QPointer<QValueAxis>  axisX;
    QPointer<QValueAxis>  axisY;
    QPointer<QLineSeries> lineSeries;
    QPointer<EggSensor>   sensor;
};

EggModel::EggModel(QObject *parent) :
    QObject(parent), dPtr(new EggModelPrivate)
{
    dPtr->uiTimer.setTimerType(Qt::PreciseTimer);
    dPtr->uiTimer.setInterval(screenRefreshRateMS());

    connect(&dPtr->uiTimer, &QTimer::timeout, this, &Self::repaint);
}

EggModel::~EggModel()
{
    dPtr->sensor->requestInterruption();
    dPtr->sensor->quit();
    dPtr->sensor->wait();
}

void
EggModel::start()
{
    // dPtr->lineSeries->blockSignals(true);
    dPtr->sensor->setBufferSize(visualPointsCount());
    dPtr->batchBuf.reserve(screenRefreshRateMS());
    dPtr->points.reserve(visualPointsCount() + screenRefreshRateMS());
    dPtr->sensor->start(QThread::HighestPriority);
    dPtr->uiTimer.start();
}

void
EggModel::setAxisX(QValueAxis *xAxis)
{
    dPtr->axisX = xAxis;
}

QValueAxis *
EggModel::axisX() const
{
    return dPtr->axisX;
}

void
EggModel::setAxisY(QValueAxis *yAxis)
{
    dPtr->axisY = yAxis;
}

QValueAxis *
EggModel::axisY() const
{
    return dPtr->axisY;
}

void
EggModel::setSeries(QLineSeries *series)
{
    dPtr->lineSeries = series;
}

QLineSeries *
EggModel::series() const
{
    return dPtr->lineSeries;
}

void
EggModel::setSensor(EggSensor *newSensor)
{
    dPtr->sensor = newSensor;
    dPtr->sensor->setParent(this);
}

EggSensor *
EggModel::sensor() const
{
    return dPtr->sensor;
}

void
EggModel::repaint()
{
    // ~0 ms
    ScopedTimeTraceMS trace("Main   Thread");

    if(!dPtr->lineSeries) return;

    fetchBatchFromSensor();

    if(dPtr->batchBuf.empty())
    {
        return updateAxisRanges();
    }

    updateGraph();
    updateAxisRanges();
}

qreal
EggModel::windowsSize() const
{
    static auto windowSize = (dPtr->axisX->max() - dPtr->axisX->min());
    return windowSize;
}

qreal
EggModel::screenRefreshRateMS() const
{
    QScreen *screen = QGuiApplication::primaryScreen();
    return 1'000 / screen->refreshRate();
}

void
EggModel::updateAxisRanges()
{
    qreal now = dPtr->sensor->now().count();

    if((now - windowsSize()) < 0)
        dPtr->axisX->setRange(0, windowsSize());
    else
        dPtr->axisX->setRange(now - windowsSize(), now);
}

void
EggModel::fetchBatchFromSensor()
{
    // این دیگه تا 1 میلی ثانیه رو هم اکی باید بتونه بده
    const size_t maxBatch = screenRefreshRateMS();
    dPtr->batchBuf.clear();
    dPtr->sensor->buffer()->popBulk(dPtr->batchBuf, maxBatch);
}

qsizetype
EggModel::visualPointsCount() const
{
    if(!dPtr->sensor) return 0;

    qreal win = windowsSize();

    return static_cast<qsizetype>(win);
}

void
EggModel::updateGraph()
{
    qreal now         = dPtr->sensor->now().count();
    qreal minVisibleX = relu(now - windowsSize());

    dPtr->points      = dPtr->lineSeries->points() + dPtr->batchBuf;

    int removeCount   = 0;
    for(int i = 0; i < dPtr->points.size(); ++i)
    {
        if(dPtr->points.at(i).x() >= minVisibleX)
        {
            removeCount = i;
            break;
        }

        if(i == dPtr->points.size() - 1) removeCount = dPtr->points.size();
    }

    if(removeCount > 0)
    {
        QVector<QPointF> tail;
        tail.reserve(dPtr->points.size() - removeCount);

        for(int i = removeCount; i < dPtr->points.size(); ++i)
            tail.append(dPtr->points.at(i));

        dPtr->lineSeries->replace(tail);
    }
    else
    {
        dPtr->lineSeries->replace(dPtr->points);
    }

    dPtr->lineSeries->update();
    dPtr->batchBuf.clear();
}


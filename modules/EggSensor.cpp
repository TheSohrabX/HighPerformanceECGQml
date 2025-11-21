#include "EggSensor.h"

#include "modules/TimeTrace.h"

#include <cmath>
#include <random>

#include <QDebug>

struct EggSensorPrivate
{
    std::mt19937                        rng;
    qreal                               sampleRate;
    std::normal_distribution<qreal>     normalNoise {0.0, 0.01};
    std::normal_distribution<qreal>     runningNoise {0.0, 0.03};
    std::normal_distribution<qreal>     deseaseNoise {0.0, 0.05};
    EggSensor::EggMode                  mode;
    EggSensor::TimePoint                startPoint;
    std::chrono::milliseconds           period;
    QScopedPointer<RingBuffer<QPointF>> buffer;
};

EggSensor::EggSensor(QObject *parent) :
    QThread {parent}, dPtr {new EggSensorPrivate}
{
    dPtr->mode = EggSensor::EggMode::Normal;
    dPtr->rng.seed(std::random_device {}());
}

EggSensor::~EggSensor() {}

void
EggSensor::run()
{
    dPtr->startPoint = Clock::now();
    TimePoint next   = Clock::now();

    while(!QThread::currentThread()->isInterruptionRequested())
    {
        // ~0 ms
        ScopedTimeTraceMS trace("Sensor Thread");
        next        += dPtr->period;

        qreal time   = elapsedSinceStart().count();
        qreal value  = synthSample(time);

        dPtr->buffer->push(QPointF(time, value));

        auto now = Clock::now();
        if(now + std::chrono::milliseconds(1) < next)
        {
            auto sleepDuration = (next - Clock::now());
            QThread::msleep(sleepDuration.count() / 1e6);
        }

        while(!QThread::currentThread()->isInterruptionRequested() &&
              Clock::now() < next)
        {}
    }
}

RingBuffer<QPointF> *
EggSensor::buffer() const
{
    return dPtr->buffer.get();
}

std::chrono::milliseconds
EggSensor::period() const
{
    return dPtr->period;
}

void
EggSensor::setPeriod(const std::chrono::milliseconds &newPeriod)
{
    dPtr->period     = newPeriod;
    dPtr->sampleRate = 1000.0 / static_cast<qreal>(newPeriod.count());
}

qreal
EggSensor::sampleRate() const
{
    return dPtr->sampleRate;
}

void
EggSensor::setSampleRate(qreal newSampleRate)
{
    dPtr->sampleRate = newSampleRate;
    dPtr->period =
      std::chrono::milliseconds(static_cast<int>(1000.0 / newSampleRate));
}

void
EggSensor::setMode(EggMode newMode)
{
    dPtr->mode = newMode;
}

void
EggSensor::setBufferSize(size_t newSize)
{
    dPtr->buffer.reset(new RingBuffer<QPointF>(newSize));
}

size_t
EggSensor::bufferSize() const
{
    return dPtr->buffer->capacity();
}

EggSensor::EggMode
EggSensor::mode() const
{
    return dPtr->mode;
}

std::chrono::milliseconds
EggSensor::now() const
{
    return elapsedSinceStart();
}

std::chrono::milliseconds
EggSensor::elapsedSinceStart() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
      Clock::now() - dPtr->startPoint);
}

qreal
EggSensor::synthSample(qreal t)
{
    switch(dPtr->mode)
    {
        case EggMode::Normal :
            return synthNormal(t);

        case EggMode::Running :
            return synthRunning(t);

        case EggMode::HeartAttack :
            return synthHeartAttack(t);
    }

    return 0.0;
}

qreal
EggSensor::synth(qreal t, qreal period, std::normal_distribution<qreal> noise)
{
    static const qreal normPeriod =
      std::ceil(period / dPtr->period.count());    // MS

    static qreal       spike   = 0.0;
    static std::size_t counter = 0;

    if(normPeriod == counter)
    {
        counter = 0;
        spike   = 3 + noise(dPtr->rng);
    }
    else
    {
        spike -= spike / normPeriod + noise(dPtr->rng);
    }

    counter++;
    return (spike - 2);
}

qreal
EggSensor::synthNormal(qreal t)
{
    return synth(t, 800, dPtr->normalNoise);
}

qreal
EggSensor::synthRunning(qreal t)
{
    return synth(t, 600, dPtr->runningNoise);
}

qreal
EggSensor::synthHeartAttack(qreal t)
{
    return synth(t, 400, dPtr->deseaseNoise);
}


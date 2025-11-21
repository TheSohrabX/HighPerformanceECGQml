#pragma once

#include "RingBuffer.h"

#include <random>

#include <QPointF>
#include <QThread>

struct EggSensorPrivate;

class EggSensor : public QThread
{
    Q_OBJECT

public:
    using Self      = EggSensor;
    using TimePoint = std::chrono::steady_clock::time_point;
    using Clock     = std::chrono::high_resolution_clock;

public:
    enum class EggMode
    {
        Normal,
        Running,
        HeartAttack
    };
    Q_ENUM(EggMode);

public:
    explicit EggSensor(QObject *parent = nullptr);
    ~EggSensor() override;

    void                      run() override;

    RingBuffer<QPointF>      *buffer() const;

    std::chrono::milliseconds period() const;
    void  setPeriod(const std::chrono::milliseconds &newPeriod);

    qreal sampleRate() const;
    void  setSampleRate(qreal newSampleRate);

    EggMode mode() const;
    void    setMode(EggMode newMode);

    std::size_t bufferSize() const;
    void        setBufferSize(std::size_t newSize);

    std::chrono::milliseconds now() const;

private:
    std::chrono::milliseconds elapsedSinceStart() const;
    qreal                     synthSample(qreal t);
    qreal synth(qreal t, qreal period, std::normal_distribution<qreal> noise);
    qreal synthNormal(qreal t);
    qreal synthRunning(qreal t);
    qreal synthHeartAttack(qreal t);

private:
    QScopedPointer<EggSensorPrivate> dPtr;

    // clang-format off
    Q_PROPERTY(EggMode     mode        READ mode        WRITE setMode)
    Q_PROPERTY(qreal       sampleRate  READ sampleRate  WRITE setSampleRate)
    Q_PROPERTY(std::size_t  bufferSize  READ bufferSize  WRITE setBufferSize)
};


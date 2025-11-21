#include "AudioVisualizerModel.h"

#include "modules/AudioInput.h"
#include "modules/TimeTrace.h"

#include <QBarSet>

struct AudioVisualizerModelPrivate
{
    inline static constexpr const int barCount = 64;
    AudioInput                        audioInput;
    QPointer<QBarSeries>              barSeries;
    QPointer<QValueAxis>              axisX;
    QPointer<QValueAxis>              axisY;
};

AudioVisualizerModel::AudioVisualizerModel(QObject* parent) :
    QObject {parent}, dPtr {new AudioVisualizerModelPrivate}
{
    connect(&dPtr->audioInput, &AudioInput::newSpectrum, this,
            &Self::onNewSpectrum);
}

AudioVisualizerModel::~AudioVisualizerModel() {}

void
AudioVisualizerModel::start()
{
    dPtr->audioInput.start();
}

void
AudioVisualizerModel::stop()
{
    dPtr->audioInput.stop();
}

QBarSeries*
AudioVisualizerModel::barSeries() const
{
    return dPtr->barSeries;
}

void
AudioVisualizerModel::setBarSeries(QBarSeries* series)
{
    dPtr->barSeries = series;
}

QValueAxis*
AudioVisualizerModel::axisX() const
{
    return dPtr->axisX;
}

void
AudioVisualizerModel::setAxisX(QValueAxis* axis)
{
    dPtr->axisX = axis;
}

QValueAxis*
AudioVisualizerModel::axisY() const
{
    return dPtr->axisY;
}

void
AudioVisualizerModel::setAxisY(QValueAxis* axis)
{
    dPtr->axisY = axis;
}

void
AudioVisualizerModel::onNewSpectrum(const QVector<float>& magnitudes)
{
    ScopedTimeTraceMS trace(Q_FUNC_INFO);
    dPtr->barSeries->replace(downSample(magnitudes));
}

QVector<QBarSet*>
AudioVisualizerModel::downSample(const QVector<float>& magnitudes)
{
    QVector<QBarSet*> result;
    result.reserve(dPtr->barCount);

    const int step = std::floor(magnitudes.size() / dPtr->barCount);

    for(int i = 0; i < dPtr->barCount; i++)
    {
        float sum = 0.0f;

        for(int j = 0; j < step; j++)
            sum += magnitudes[i * step + j];

        float    avg = sum;    // / step;

        // leave memory leak baghiat and salehat to Qt :)
        QBarSet* set = new QBarSet(QString::number(i), dPtr->barSeries);
        set->append(avg);

        result.push_back(set);
    }

    return result;
}


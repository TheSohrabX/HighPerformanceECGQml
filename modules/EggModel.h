#pragma once

#include "EggSensor.h"

#include <QValueAxis>
#include <QtGraphs/QLineSeries>

struct EggModelPrivate;

class EggModel : public QObject
{
    Q_OBJECT

    using Self = EggModel;

public:

    struct EggSample
    {
        double t;    // time in seconds
        double y;    // amplitude
    };

public:
    explicit EggModel(QObject* parent = nullptr);
    ~EggModel() override;

    std::vector<EggSample>   generate(int N);

    Q_INVOKABLE void         start();

    void                     setAxisX(QValueAxis* xAxis);
    QValueAxis*              axisX() const;

    void                     setAxisY(QValueAxis* yAxis);
    QValueAxis*              axisY() const;

    void                     setSeries(QLineSeries* series);
    QLineSeries*             series() const;

    void                     setSensor(EggSensor* newSensor);
    EggSensor*               sensor() const;

Q_SIGNALS:
    void newSamplesGenerated(qreal sample, qreal time);
    void sampleRateChanged();
    void modeChanged();

private Q_SLOTS:
    void repaint();

private:
    qreal     windowsSize() const;
    qreal     screenRefreshRateMS() const;
    void      updateAxisRanges();
    void      fetchBatchFromSensor();
    qsizetype visualPointsCount() const;
    void      updateGraph();

    inline constexpr qreal
    relu(qreal x)
    {
        return x > 0.0 ? x : 0.0;
    }


private:
    EggModelPrivate* dPtr;

    // clang-format off
    Q_PROPERTY(EggSensor*   sensor      READ sensor     WRITE setSensor)
    Q_PROPERTY(QLineSeries* series      READ series     WRITE setSeries)
    Q_PROPERTY(QValueAxis*  axisX       READ axisX      WRITE setAxisX)
    Q_PROPERTY(QValueAxis*  axisY       READ axisY      WRITE setAxisY)
};


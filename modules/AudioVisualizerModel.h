#pragma once

#include <QBarSeries>
#include <QObject>
#include <QPointer>
#include <QValueAxis>

struct AudioVisualizerModelPrivate;

class AudioVisualizerModel : public QObject
{
    Q_OBJECT

    using Self = AudioVisualizerModel;

public:
    explicit AudioVisualizerModel(QObject* parent = nullptr);
    ~AudioVisualizerModel() override;

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

    QBarSeries*      barSeries() const;
    void             setBarSeries(QBarSeries* series);

    QValueAxis*      axisX() const;
    void             setAxisX(QValueAxis* axis);

    QValueAxis*      axisY() const;
    void             setAxisY(QValueAxis* axis);

private Q_SLOTS:
    void onNewSpectrum(const QVector<float>& magnitudes);

private:
    QVector<QBarSet*> downSample(const QVector<float>& magnitudes);

private:
    QScopedPointer<AudioVisualizerModelPrivate> dPtr;

private:
    Q_PROPERTY(QBarSeries* barSeries READ barSeries WRITE setBarSeries)
    Q_PROPERTY(QValueAxis* axisX READ axisX WRITE setAxisX)
    Q_PROPERTY(QValueAxis* axisY READ axisY WRITE setAxisY)
};


#pragma once

#include <fftw3.h>

#include <QAudioDevice>
#include <QAudioInput>
#include <QAudioSource>
#include <QIODevice>
#include <QMediaDevices>
#include <QObject>

struct AudioInputPrivate;

class AudioInput : public QIODevice
{
    Q_OBJECT

public:
    explicit AudioInput(QObject *parent = nullptr);
    ~AudioInput() override;

    void start();
    void stop();

Q_SIGNALS:
    void newSpectrum(const QVector<float> &magnitudes);

private Q_SLOTS:
    void handleStateChanged(QAudio::State state);

protected:
    qint64 writeData(const char *data, qint64 len) override;
    qint64 readData(char *data, qint64 maxlen) override;

private:
    void initFFTW();
    void computeFFT();

private:    // members
    QScopedPointer<AudioInputPrivate> dPtr;
};

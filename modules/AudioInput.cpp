#include "AudioInput.h"

#include <QDebug>

struct AudioInputPrivate
{
    QAudioSource        *audioSource;
    int                  bitRate      = 48'000;
    int                  channelCount = 1;

    static constexpr int FFT_SIZE     = 1'024;

    QVector<double>      windowBuf;
    int                  writePos = 0;

    fftw_plan            plan;
    double              *in  = nullptr;
    fftw_complex        *out = nullptr;
};

AudioInput::AudioInput(QObject *parent) :
    QIODevice {parent}, dPtr {new AudioInputPrivate}
{
    QAudioFormat format;
    format.setSampleRate(dPtr->bitRate);
    format.setChannelCount(dPtr->channelCount);
    format.setSampleFormat(QAudioFormat::Int16);

    QAudioDevice info = QMediaDevices::defaultAudioInput();
    if(!info.isFormatSupported(format))
    {
        qWarning()
          << "Default format not supported, trying to use the nearest.";

        format = info.preferredFormat();
    }

    dPtr->audioSource = new QAudioSource(format, this);

    initFFTW();

    connect(dPtr->audioSource,
            &QAudioSource::stateChanged,
            this,
            &AudioInput::handleStateChanged);
}

AudioInput::~AudioInput()
{
    dPtr->audioSource->stop();
    dPtr->audioSource->deleteLater();
    close();

    fftw_destroy_plan(dPtr->plan);
    fftw_free(dPtr->in);
    fftw_free(dPtr->out);
}

void
AudioInput::start()
{
    open(QIODevice::WriteOnly);
    dPtr->audioSource->start(this);
}

void
AudioInput::stop()
{
    dPtr->audioSource->stop();
}

void
AudioInput::handleStateChanged(QAudio::State state)
{

    switch(state)
    {
        case QAudio::StoppedState :
            if(dPtr->audioSource->error() != QAudio::NoError)
            {
                qCritical()
                  << "Audio source error: " << dPtr->audioSource->error();
            }
            else
            {
                qCritical() << "Audio source stopped.";
            }
            break;

        case QAudio::ActiveState :
            qCritical() << "Audio source active.";
            break;

        default :
            qCritical() << "Audio source state: " << state;
            break;
    }
}

qint64
AudioInput::writeData(const char *data, qint64 len)
{
    QByteArray     packet(data, len);

    const int16_t *samples     = reinterpret_cast<const int16_t *>(data);
    const int      sampleCount = len / sizeof(int16_t);

    for(int i = 0; i < sampleCount; i++)
    {
        if(dPtr->writePos >= dPtr->windowBuf.size())
        {
            dPtr->writePos = 0;
        }

        dPtr->windowBuf[dPtr->writePos] = samples[i] / 32768.0;
        dPtr->writePos++;

        if(dPtr->writePos >= dPtr->FFT_SIZE)
        {
            dPtr->writePos = 0;
            computeFFT();
        }
    }

    return len;
}

qint64
AudioInput::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)
    return -1;
}

void
AudioInput::initFFTW()
{
    dPtr->windowBuf = QVector<double>(dPtr->FFT_SIZE);
    dPtr->writePos  = 0;

    dPtr->in       = (double *)fftw_malloc(sizeof(double) * dPtr->FFT_SIZE);
    dPtr->out      = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) *
                                                 (dPtr->FFT_SIZE / 2 + 1));

    dPtr->plan =
      fftw_plan_dft_r2c_1d(dPtr->FFT_SIZE, dPtr->in, dPtr->out, FFTW_MEASURE);
}

void
AudioInput::computeFFT()
{
    for(int i = 0; i < dPtr->FFT_SIZE; i++)
        dPtr->in[i] = dPtr->windowBuf[i];

    fftw_execute(dPtr->plan);

    QVector<float> magnitudes;
    magnitudes.reserve(dPtr->FFT_SIZE / 2);

    for(int k = 0; k < dPtr->FFT_SIZE / 2; k++)
    {
        double real = dPtr->out[k][0];
        double imag = dPtr->out[k][1];

        float  mag  = sqrt(real * real + imag * imag);

        magnitudes.push_back(mag);
    }

    Q_EMIT newSpectrum(magnitudes);
}

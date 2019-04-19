#ifndef AUDIO_H
#define AUDIO_H

//windows下的CoreAudio技术

#include <Windows.h>
#include <initguid.h>
#include <MMDeviceAPI.h>
#include <AudioClient.h> //WSAPI
#include <AudioPolicy.h> //WSAPI
#include <QObject>


class Audio: public QObject
{
    Q_OBJECT

public:
    Audio();
    ~Audio();

    BYTE *pBuffer = NULL;

    void initAudio();
    void uninitAudio();

signals:
    void collectData(QByteArray );

public slots:
    void record();
    void stopRecord();

private:
    void startRecord();

    IAudioClient *      _AudioClient = NULL;
    IAudioCaptureClient *_CaptureClient = NULL;
    IMMDevice * _Device = NULL;
    IMMDeviceEnumerator * _DeviceEnumerator = NULL;
    HANDLE _AudioSamplesReadyEvent = NULL;
    WAVEFORMATEX *      _MixFormat = NULL;
    HRESULT hr;
    size_t _FrameSize;


//    BYTE *pBuffer;
    bool recording;
};

#endif // AUDIO_H

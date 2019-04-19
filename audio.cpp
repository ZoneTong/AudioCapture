#include "audio.h"

#include <QDebug>

#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

#define MAX_AUDIO_FRAME_SIZE 192000
#define SAFE_RELEASE(punk) \
            if((NULL != punk)) \
            { (punk)->Release(); (punk)=NULL; }


Audio::Audio()
{

}

Audio::~Audio()
{

}

void Audio::initAudio()
{
    //初始化Com库
    CoUninitialize();
    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if(hr != S_OK)
    {
        qDebug() << QString("初始化Com库失败\n");
        return ; //-1
    }
    //创建Com对象IMMDeviceEnumerator
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_DeviceEnumerator));
//    hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL,CLSCTX_ALL, IID_IMMDeviceEnumerator,(void**)&_DeviceEnumerator);
//    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,CLSCTX_INPROC_SERVER, IID_IX,(void**)&_DeviceEnumerator);
    if (FAILED(hr))
    {
       qDebug() << QString("Unable to retrieve CoCreateInstance %1\n").arg( (long)hr);
       return ;
    }
    //获取声音播放设备对象IMMDevice
    hr = _DeviceEnumerator->GetDefaultAudioEndpoint(eRender,eMultimedia,&_Device);
    if (FAILED(hr))
    {
       qDebug() << (QString("Unable to retrieve device %1\n").arg( hr));
       return ;
    }
    //释放IMMDeviceEnumerator对象
    SAFE_RELEASE(_DeviceEnumerator)
    //在音频数据拷贝之前要添加一个引用,
    _Device->AddRef();
    //创建Com对象IAudioClient
   hr = _Device->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void **>(&_AudioClient));
   if (FAILED(hr))
   {
       qDebug() << (QString("Unable to activate audio client: %1.\n").arg( hr));
       return ;
   }
   //获取音频引擎内如共享模式程序的音频流格式
   hr = _AudioClient->GetMixFormat(&_MixFormat);
   if (FAILED(hr))
   {
       qDebug() << (QString("Unable to get mix format on audio client: %1.\n").arg( hr));
       return ;
   }
   //wBitsPerSample是采样深度(位深)  nChannels是音频通道数 _FrameSize一个采样的大小(字节)
   _FrameSize = (_MixFormat->wBitsPerSample / 8) * _MixFormat->nChannels;

   qDebug() << QString("audio _MixFormat wBitsPerSample: %1, nChannels: %2, nSamplesPerSec: %3, nBlockAlign: %4, FrameSize: %5").arg(_MixFormat->wBitsPerSample)
               .arg(_MixFormat->nChannels).arg(_MixFormat->nSamplesPerSec).arg(_MixFormat->nBlockAlign).arg(_FrameSize);
   //初始化音频引擎
   /*
    *AUDCLNT_SHAREMODE_SHARED只用共享模式才能在还回(loopback)模式下起作用
    *AUDCLNT_STREAMFLAGS_EVENTCALLBACK允许设置事件通知回调 SetEventHandle才会有效果
    *AUDCLNT_STREAMFLAGS_LOOPBACK设置音频为还回模式,
    *这样WSAAPI客户端就能捕获渲染断点设备播放的音频流
    */
   hr = _AudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK|AUDCLNT_STREAMFLAGS_EVENTCALLBACK, 0, 0, _MixFormat, NULL);
   if (FAILED(hr))
   {
       qDebug() << (QString("Unable to initialize audio client: %1.\n").arg( hr));
       return ;
   }
   _AudioSamplesReadyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
   if (_AudioSamplesReadyEvent == NULL)
   {
        qDebug() << QString("Unable to create samples ready event: %s.\n"   );
//       qDebug() << (QString("Unable to create samples ready event: %s.\n", GetLastError()));
       return ;
   }
   //设置事件通知对象
   hr = _AudioClient->SetEventHandle(_AudioSamplesReadyEvent);
   if (FAILED(hr))
   {
       qDebug() << (QString("Unable to set ready event: %1.\n").arg( hr));
       return ;
   }


   hr = _AudioClient->GetBufferSize(&bufferFrameCount);
   if (FAILED(hr))
   {
       qDebug() << (QString("Unable to GetBufferSize: %1.\n").arg( hr));
       return;
   }
   qDebug() << "bufferFrameCount" << bufferFrameCount;

   //生成采集服务
   hr = _AudioClient->GetService(IID_PPV_ARGS(&_CaptureClient));
   if (FAILED(hr))
   {
       qDebug() << (QString("Unable to get new capture client: %1.\n").arg( hr));
       return ;
   }

   startRecord();
}


void Audio::startRecord()
{
   //开始采集
   qDebug() << "before _AudioClient Start!";
   hr = _AudioClient->Start();
   qDebug() << "after  _AudioClient Start!";
   if (FAILED(hr))
   {
       qDebug() << (QString( "Unable to get new capture client: %1.\n").arg( hr));
       return;
   }
}

void Audio::record()
{
   if (pBuffer != NULL){
        delete []pBuffer;
   }
   pBuffer=new BYTE[MAX_AUDIO_FRAME_SIZE];
   UINT32 buffer_len = 0;

   REFERENCE_TIME hnsActualDuration = (double)REFTIMES_PER_SEC * bufferFrameCount / _MixFormat->nSamplesPerSec;
   DWORD tmp_time = hnsActualDuration / REFTIMES_PER_MILLISEC / 2;


   DWORD last_time = 0;
   recording = true;
   while (last_time <= 400)
   {
       Sleep(tmp_time);
       last_time += tmp_time;

       qDebug() << last_time  << "recording!";
//       DWORD waitResult = WaitForSingleObject(_AudioSamplesReadyEvent, INFINITE); //INFINITE应该是一直阻塞到读取到声音
//       WaitForSingleObject(_AudioSamplesReadyEvent, 3000);



       UINT32 uiNextPacketSize(0);
       hr = _CaptureClient->GetNextPacketSize(&uiNextPacketSize);
       if (FAILED(hr))
       {
           qDebug() << "GetNextPacketSize FAILED!";
           break;
        }

       while(uiNextPacketSize != 0){

       BYTE *pData = NULL;
//       INT nBufferLenght = 0;
       UINT32 framesAvailable = 0;
       DWORD  flags = 0;
       hr = _CaptureClient->GetBuffer(&pData, &framesAvailable, &flags, NULL, NULL);
       if (SUCCEEDED(hr))
       {
           if (framesAvailable!=0)
           {
               if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
               {
                   //告诉Copy Data写静音
                   pData = NULL;
               }
               else
               {
                   //Copy data from the audio engine buffer to the output buffer.
                   int nDataLen = framesAvailable*_FrameSize;
                   qDebug() << QString("nDataLen: %1, framesAvailable %2, MAX_AUDIO_FRAME_SIZE: %3").arg(nDataLen).arg(framesAvailable).arg(MAX_AUDIO_FRAME_SIZE);
                   CopyMemory(pBuffer + buffer_len,pData,nDataLen);
                   qDebug() << QString("get capture audio len: %1: %2!\n").arg( nDataLen).arg(QString(QByteArray((char *)pBuffer+buffer_len, 5)));
                   buffer_len += nDataLen;
//                   emit collectData(QByteArray((char *)pBuffer, nDataLen));

               }
           }
       }
       _CaptureClient->ReleaseBuffer(framesAvailable);


       hr = _CaptureClient->GetNextPacketSize(&uiNextPacketSize);
       if (FAILED(hr))
           break;
       }
   }

   emit collectData(QByteArray((char *)pBuffer, buffer_len));
   qDebug() << "recorded!";
}

void Audio::stopRecord()
{
   recording = false;
}

void Audio::uninitAudio(){
    if (pBuffer != NULL){
         delete []pBuffer;
    }
    SAFE_RELEASE(_DeviceEnumerator)
    SAFE_RELEASE(_Device)
    SAFE_RELEASE(_DeviceEnumerator)
    SAFE_RELEASE(_DeviceEnumerator)
    SAFE_RELEASE(_DeviceEnumerator)
    CoUninitialize();
}


//-----------------------------------------------------------------------------
//
//      Библиотека для работы со звуком
//      (c) РГУПС, ВЖД 24/03/2017
//      Разработал: Ковшиков С. В.
//
//-----------------------------------------------------------------------------


#ifndef ASOUND_H
#define ASOUND_H

#include <QObject>
#include <QFile>
#include <cstdlib>
#include <iostream>
#include <al.h>
#include <alc.h>
#include "asound_global.h"


#pragma pack(push, 1)
struct wave_info_t
{
    char            chunkId[4];     // ИД главного фрагмента "RIFF"
    uint32_t        chunkSize;      // Размер первого фрагмента
    char            format[4];      // Формат "WAVE"
    char            subchunk1Id[4]; // ИД первого подфрагмента "fmt"
    uint32_t        subchunk1Size;  // Размер первого подфрагмента
    short           audioFormat;    // Формат сжатия
    short           numChannels;    // Количество каналов
    uint32_t        sampleRate;     // Частота дискретизации (frequency)
    uint32_t        byteRate;       // Байт в секунду
    short           bytesPerSample; // Байт в одном сэмпле (blockAlign)
    short           bitsPerSample;  // Бит в сэмпле
    char            subchunk2Id[4]; // ИД второго субфрагмента "data"
    uint32_t        subchunk2Size;  // Размер самого звука
// Constructor
    wave_info_t()
    {
        strcpy(chunkId, "");
        chunkSize = 0;
        strcpy(format, "");
        strcpy(subchunk1Id, "");
        subchunk1Size = 0;
        audioFormat = 0;
        numChannels = 0;
        sampleRate = 0;
        byteRate = 0;
        bytesPerSample = 0;
        bitsPerSample = 0;
        strcpy(subchunk2Id, "");
        subchunk2Size = 0;
    }
};
#pragma pack(pop)

// Скорость воспроизведения источника по умолчанию
const float DEF_PITCH = 1.0f;

// Громкость
const int MIN_VOLUME  = 0;
const int DEF_VOLUME  = 100;
const int MAX_VOLUME  = 100;

// Положение источника по умолчанию
const float DEF_POS[] = {0.0, 0.0, 1.0};
// "Скорость передвижения" источника по умолчанию
const float DEF_VEL[] = {0.0, 0.0, 0.0};


class ASOUNDSHARED_EXPORT ASound : public QObject
{
    Q_OBJECT
public:
    ASound(QString soundname, QObject* parent = Q_NULLPTR);
    ~ASound();

//! SETTERS СЕТТЕРЫ //
    // Установка громкости
    void setVolume(int volume = 100);
    // Установка скорости воспроизведения
    void setPitch(float pitch);
    // Установка зацикливания
    void setLoop(bool loop);
    // Установка позиции
    void setPosition(float x, float y, float z);
    // Установка "скорости передвижения"
    void setVelocity(float x, float y, float z);

//! GETTERS ГЕТТЕРЫ //
    // Получить громкость
    int getVolume();
    // Получить скорость воспроизведения
    float getPitch();
    // Получить зацикливание
    bool getLoop();
    // Получить позицию
    void getPosition(float &x, float &y, float &z);
    // Получить скорость
    void getVelocity(float &x, float &y, float &z);
    // Получить последнюю ошибку
    QString getLastError();

public slots:
    // Играть звук
    void play();
    // Преостановить звук
    void pause();
    // Остановить звук
    void stop();
    // Установить громкость
    void setVolumeS(int vol);
    // Установить зацикливание
    void setLoopS(bool loop);

private:
    // Можно продолжать работу с файлом
    bool canDo_;
    // Можно играть звук
    bool canPlay_;

    // Имя звука
    QString soundName_;
    // Последняя ошибка
    QString lastError_;

    // Переменная для хранения файла
    QFile file_;

    // Информация о файле .wav
    wave_info_t wave_info_;
    // Хранилище для data секции (самой музыки) файла .wav
    unsigned char* wavData_;
    // Буфер OpenAL
    ALuint  buffer_;
    // Источник OpenAL
    ALuint  source_;
    // Формат аудио (mono8/16 - stereo8/16) OpenAL
    ALenum  format_;

    // Хранение громкости
    int sourceVolume_;
    // Хранение скорости воспроизведения
    ALfloat sourcePitch_;
    // Хранение зацикливания
    bool sourceLoop_;
    // Хранение положения источника
    ALfloat sourcePosition_[3];
    // Хранение "скорости передвижения" источника
    ALfloat sourceVelocity_[3];

    // Полная подготовка файла
    void loadSound_(QString soundname);
    // Загрузка файла (в т.ч. из ресурсов)
    void loadFile_(QString soundname);
    // Чтение информации в файле .wav
    void readWaveInfo_();
    // Определение формата аудио (mono8/16 - stereo8/16)
    void defineFormat_();
    // Генерируем буфер и источник
    void generateStuff_();
    // Настраиваем источник
    void configureSource_();
};

#endif // ASOUND_H

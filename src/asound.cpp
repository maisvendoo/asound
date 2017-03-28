//-----------------------------------------------------------------------------
//
//      Библиотека для работы со звуком
//      (c) РГУПС, ВЖД 24/03/2017
//      Разработал: Ковшиков С. В.
//
//-----------------------------------------------------------------------------


#include "asound.h"


//-----------------------------------------------------------------------------
// КОНСТРУКТОР
//-----------------------------------------------------------------------------
ASound::ASound(QString soundname, QObject *parent): QObject(parent),
    soundName_(soundname),      // Сохраняем название звука
    canDo_(false),              // Сбрасываем флаг
    canPlay_(false),            // Сбрасываем флаг
    format_(0),                 // Обнуляем формат
    source_(0),                 // Обнуляем источник
    buffer_(0),                 // Обнуляем буффер
    wavData_(NULL),             // Обнуляем массив дорожки
    sourceVolume_(DEF_VOLUME),  // Громкость по умол.
    sourcePitch_(DEF_PITCH),    // Скорость воспроизведения по умолч.
    sourceLoop_(false),         // Зацикливание по-умолч.
    sourcePosition_{DEF_POS[0], // Позиция по умолч
                    DEF_POS[1], //
                    DEF_POS[2]},//
    sourceVelocity_{DEF_VEL[0], // Скорость передвижения по умолч.
                    DEF_VEL[1], //
                    DEF_VEL[2]} //
{
    // Загружаем звук
    loadSound_(soundname);
}



//-----------------------------------------------------------------------------
// ДЕСТРУКТОР
//-----------------------------------------------------------------------------
ASound::~ASound()
{
    if (wavData_)
    {
        delete[] wavData_;
    }
    alDeleteSources(1, &source_);
    alDeleteBuffers(1, &buffer_);
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ASound::loadSound_(QString soundname)
{
    // Сбрасываем флаги
    canDo_ = false;
    canPlay_ = false;
    // Сохраняем название звука
    soundName_ = soundname;
    // Загружаем файл
    loadFile_(soundname);
    // Читаем информационный раздел 44байта
    readWaveInfo_();
    // Определяем формат аудио (mono8/16 - stereo8/16) OpenAL
    defineFormat_();
    // Генерируем буфер и источник
    generateStuff_();
    // Настраиваем источник
    configureSource_();
    // Можно играть звук
    if (canDo_)
    {
        canPlay_ = true;
    }
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ASound::loadFile_(QString soundname)
{
    file_.setFileName(soundname);

    if (!file_.exists())
    {
        lastError_ = "NO_SUCH_FILE: ";
        lastError_.append(soundname);
        canDo_ = false;
        return;
    }

    if (file_.open(QIODevice::ReadOnly))
    {
        canDo_ = true;
    }
    else
    {
        canDo_ = false;
        lastError_ = "CANT_OPEN_FILE_FOR_READING: ";
        lastError_.append(soundname);
        return;
    }
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ASound::readWaveInfo_()
{
    if (canDo_)
    {
        if (wavData_)
        {
            delete[] wavData_;
        }

        QByteArray arr = file_.read(sizeof(wave_info_t));

        memcpy((unsigned char*) &wave_info_,
               (unsigned char*) arr.data(),
               sizeof(wave_info_t));

        //! /////////////////////////////////////////// //
        //!    Далее идет проверка данных. Проверка     //
        //!  производится ПОСИМВОЛЬНО, так как в данных //
        //! нет символа, завершающего строку, и в конце //
        //!     строки появляются лишние знаки!!!       //
        //! /////////////////////////////////////////// //

        // Проверка ИД главного раздела -    -    -    -    -    -    RIFF
        bool foo =  wave_info_.chunkId[0] == 'R' &&
                    wave_info_.chunkId[1] == 'I' &&
                    wave_info_.chunkId[2] == 'F' &&
                    wave_info_.chunkId[3] == 'F';
        if (!foo)
        {
            lastError_ = "NOT_RIFF_FILE";
            canDo_ = false;
            return;
        }

        // Проверка формата   -    -    -    -    -    -    -    -    WAVE
        foo =   wave_info_.format[0] == 'W' &&
                wave_info_.format[1] == 'A' &&
                wave_info_.format[2] == 'V' &&
                wave_info_.format[3] == 'E';
        if (!foo)
        {
            lastError_ = "NOT_WAVE_FILE";
            canDo_ = false;
            return;
        }

        // Проверка ИД первого подраздела     -    -    -    -    -    fmt
        foo =   wave_info_.subchunk1Id[0] == 'f' &&
                wave_info_.subchunk1Id[1] == 'm' &&
                wave_info_.subchunk1Id[2] == 't';
        if (!foo)
        {
            lastError_ = "NO_fmt";
            canDo_ = false;
            return;
        }

        // Проверка ИД второго подраздела    -    -    -    -    -    data
        foo =   wave_info_.subchunk2Id[0] == 'd' &&
                wave_info_.subchunk2Id[1] == 'a' &&
                wave_info_.subchunk2Id[2] == 't' &&
                wave_info_.subchunk2Id[3] == 'a';
        if (!foo)
        {
            lastError_ = "NO_data";
            canDo_ = false;
            return;
        }

        arr = file_.read(wave_info_.subchunk2Size);

        wavData_ = new unsigned char[wave_info_.subchunk2Size];

        memcpy((unsigned char*) wavData_,
               (unsigned char*) arr.data(),
               wave_info_.subchunk2Size);

        file_.close();
    }
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ASound::defineFormat_()
{
    if (canDo_)
    {
        if (wave_info_.bitsPerSample == 8)
        {
            if (wave_info_.numChannels == 1)
            {
                format_ = AL_FORMAT_MONO8;
            }
            else
            {
                format_ = AL_FORMAT_STEREO8;
            }
        }
        else if (wave_info_.bitsPerSample == 16)
        {
            if (wave_info_.numChannels == 1)
            {
                format_ = AL_FORMAT_MONO16;
            }
            else
            {
                format_ = AL_FORMAT_STEREO16;
            }
        }
        else
        {
            lastError_ = "UNKNOWN_AUDIO_FORMAT";
            canDo_ = false;
        }
    }
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ASound::generateStuff_()
{
    if (canDo_)
    {
        alGenBuffers(1, &buffer_);

        if (alGetError() != AL_NO_ERROR)
        {
            canDo_ = false;
            lastError_ = "CANT_GENERATE_BUFFER";
            return;
        }

        alGenSources(1, &source_);

        if (alGetError() != AL_NO_ERROR)
        {
            canDo_ = false;
            lastError_ = "CANT_GENERATE_SOURCE";
            return;
        }

        alBufferData(buffer_,
                     format_,
                     wavData_,
                     wave_info_.subchunk2Size,
                     wave_info_.sampleRate);

        if (alGetError() != AL_NO_ERROR)
        {
            canDo_ = false;
            lastError_ = "CANT_MAKE_BUFFER_DATA";
            return;
        }
    }
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ASound::configureSource_()
{
    if (canDo_)
    {
        alSourcei(source_, AL_BUFFER, buffer_);

        if (alGetError() != AL_NO_ERROR)
        {
            canDo_ = false;
            lastError_ = "CANT_ADD_BUFFER_TO_SOURCE";
            return;
        }

        alSourcef(source_, AL_GAIN, 0.01f * sourceVolume_);

        if (alGetError() != AL_NO_ERROR)
        {
            canDo_ = false;
            lastError_ = "CANT_APPLY_VOLUME";
            return;
        }

        alSourcef(source_, AL_PITCH, sourcePitch_);

        if (alGetError() != AL_NO_ERROR)
        {
            canDo_ = false;
            lastError_ = "CANT_APPLY_PITCH";
            return;
        }

        alSourcei(source_, AL_LOOPING, (char)sourceLoop_);

        if (alGetError() != AL_NO_ERROR)
        {
            canDo_ = false;
            lastError_ = "CANT_APPLY_LOOPING";
            return;
        }

        alSourcefv(source_, AL_POSITION, sourcePosition_);

        if (alGetError() != AL_NO_ERROR)
        {
            canDo_ = false;
            lastError_ = "CANT_APPLY_POSITION";
            return;
        }

        alSourcefv(source_, AL_VELOCITY, sourceVelocity_);

        if (alGetError() != AL_NO_ERROR)
        {
            canDo_ = false;
            lastError_ = "CANT_APPLY_VELOCITY";
            return;
        }
    }
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ASound::setVolume(int volume)
{
    if (canPlay_)
    {
        sourceVolume_ = volume;

        if (sourceVolume_ > 100)
            sourceVolume_ = 100;

        if (sourceVolume_ < 0)
            sourceVolume_ = 0;

        alSourcef(source_, AL_GAIN, 0.01f * sourceVolume_);
    }
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int ASound::getVolume()
{
    return sourceVolume_;
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ASound::setPitch(float pitch)
{
    if (canPlay_)
    {
        sourcePitch_ = pitch;
        alSourcef(source_, AL_PITCH, sourcePitch_);
    }
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
float ASound::getPitch()
{
    return sourcePitch_;
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ASound::setLoop(bool loop)
{
    if (canPlay_)
    {
        sourceLoop_ = loop;
        alSourcei(source_, AL_LOOPING, (char)sourceLoop_);
    }
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool ASound::getLoop()
{
    return sourceLoop_;
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ASound::setPosition(float x, float y, float z)
{
    if (canPlay_)
    {
        sourcePosition_[0] = x;
        sourcePosition_[1] = y;
        sourcePosition_[2] = z;
        alSourcefv(source_, AL_POSITION, sourcePosition_);
    }
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ASound::getPosition(float &x, float &y, float &z)
{
    x = sourcePosition_[0];
    y = sourcePosition_[1];
    z = sourcePosition_[2];
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ASound::setVelocity(float x, float y, float z)
{
    if (canPlay_)
    {
        sourceVelocity_[0] = x;
        sourceVelocity_[1] = y;
        sourceVelocity_[2] = z;
        alSourcefv(source_, AL_VELOCITY, sourceVelocity_);
    }
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ASound::getVelocity(float &x, float &y, float &z)
{
    x = sourceVelocity_[0];
    y = sourceVelocity_[1];
    z = sourceVelocity_[2];
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ASound::play()
{
    if (canPlay_)
    {
        alSourcePlay(source_);
    }
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ASound::pause()
{
    if (canPlay_)
    {
        alSourcePause(source_);
    }
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ASound::stop()
{
    if (canPlay_)
    {
        alSourceStop(source_);
    }
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ASound::setVolumeS(int vol)
{
    setVolume(vol);
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ASound::setLoopS(bool loop)
{
    setLoop(loop);
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
QString ASound::getLastError()
{
    QString foo = lastError_;
    lastError_.clear();
    return foo;
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool ASound::isPlaying()
{
    ALint state;
    alGetSourcei(source_, AL_SOURCE_STATE, &state);
    return(state == AL_PLAYING);
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool ASound::isPaused()
{
    ALint state;
    alGetSourcei(source_, AL_SOURCE_STATE, &state);
    return(state == AL_PAUSED);
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool ASound::isStopped()
{
    ALint state;
    alGetSourcei(source_, AL_SOURCE_STATE, &state);
    return(state == AL_STOPPED);
}

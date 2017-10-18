//-----------------------------------------------------------------------------
//
//      Библиотека для работы с 3D звуком
//      (c) РГУПС, ВЖД 24/03/2017
//      Разработал: Ковшиков С. В.
//
//-----------------------------------------------------------------------------
/*!
 *  \file
 *  \brief Библиотека для работы с 3D звуком
 *  \copyright РГУПС, ВЖД
 *  \author Ковшиков С. В.
 *  \date 24/03/2017
 */

#ifndef ASOUND_H
#define ASOUND_H

#include <QObject>
#include <al.h>
#include <alc.h>

class QFile;
class QTimer;


#if defined(ASOUND_LIBRARY)
#  define ASOUNDSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ASOUNDSHARED_EXPORT Q_DECL_IMPORT
#endif


//-----------------------------------------------------------------------------
// Класс AListener
//-----------------------------------------------------------------------------
/// Положение слушателя по умолчанию
const float DEF_LSN_POS[3] = {0.0f, 0.0f, 0.0f};
/// "Скорость передвижения" слушателя по умолчанию
const float DEF_LSN_VEL[3] = {0.0f, 0.0f, 0.0f};
/// Направление слушателя по умолчанию
const float DEF_LSN_ORI[6] = {0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f};

/*!
 * \class AListener
 * \brief Класс, реализующий создание единственного слушателя
 */
class ASOUNDSHARED_EXPORT AListener
{
public:
    /// Статический метод запрещающий повторное создание экземпляра класса
    static AListener &getInstance();

    ///
    void closeDevices();


private:
    /// Конструктор (priate!)
    AListener();

    /// Аудиоустройство
    ALCdevice* device_;

    /// Контекст OpenAL
    ALCcontext* context_;

    /// Положение слушателя
    ALfloat listenerPosition_[3];

    /// "Скорость передвижения" слушателя
    ALfloat listenerVelocity_[3];

    /// Направление слушателя
    ALfloat listenerOrientation_[6];

};



//-----------------------------------------------------------------------------
// Класс ASound
//-----------------------------------------------------------------------------
#pragma pack(push, 1)
/*!
 * \struct wave_info_t
 * \brief Структура для хранения данных о wav файле
 */
struct wave_info_t
{
    char            chunkId[4];     ///< ID главного фрагмента "RIFF"
    uint32_t        chunkSize;      ///< Размер первого фрагмента
    char            format[4];      ///< Формат "WAVE"
    char            subchunk1Id[4]; ///< ID первого подфрагмента "fmt"
    uint32_t        subchunk1Size;  ///< Размер первого подфрагмента
    short           audioFormat;    ///< Формат сжатия
    short           numChannels;    ///< Количество каналов
    uint32_t        sampleRate;     ///< Частота дискретизации (frequency)
    uint32_t        byteRate;       ///< Байт в секунду
    short           bytesPerSample; ///< Байт в одном сэмпле (blockAlign)
    short           bitsPerSample;  ///< Бит в сэмпле
    char            subchunk2Id[4]; ///< ID второго субфрагмента "data"
    uint32_t        subchunk2Size;  ///< Размер дорожки
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

/// Скорость воспроизведения источника по умолчанию
const float DEF_SRC_PITCH = 1.0f;

/// Минимальная громкость источника
const int MIN_SRC_VOLUME  = 0;
/// Громкость источника по умолчанию
const int DEF_SRC_VOLUME  = 100;
/// Максимальная громкость источника
const int MAX_SRC_VOLUME  = 100;

/// Положение источника по умолчанию
const float DEF_SRC_POS[3] = {0.0f, 0.0f, 1.0f};
/// "Скорость передвижения" источника по умолчанию
const float DEF_SRC_VEL[3] = {0.0f, 0.0f, 0.0f};

/*!
 * \class ASound
 * \brief Класс реализующий создание источника звука, настройки его
 * пространственных характеристик, загрузки аудиофайла в формате wav и
 * последующего воспроизведения
 */
class ASOUNDSHARED_EXPORT ASound : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Конструктор
     * \param soundname - имя аудиофайла
     */
    ASound(QString soundname, QObject* parent = Q_NULLPTR);
    /// Деструктор
    ~ASound();

    /// Вернуть громкость
    int getVolume();

    /// Вернуть скорость воспроизведения
    float getPitch();

    /// Вернуть флаг зацикливания
    bool getLoop();

    /// Вернуть положение источника
    void getPosition(float &x, float &y, float &z);

    /// Вернуть "скорость передвижения" источника
    void getVelocity(float &x, float &y, float &z);

    /// Вернуть последнюю ошибку
    QString getLastError();

    /// Играет ли звук
    bool isPlaying();

    /// Остановлен ли звук
    bool isStopped();

    /// Приостановлен ли звук
    bool isPaused();

    /// Длительность звука в секундах
    int getDuration();


public slots:
    /// Установить громкость
    void setVolume(int volume = 100);

    /// Установить скорости воспроизведения
    void setPitch(float pitch);

    /// Установить зацикливание
    void setLoop(bool loop);

    /// Установить положение
    void setPosition(float x, float y, float z);

    /// Установить "скорость передвижения"
    void setVelocity(float x, float y, float z);

    /// Играть звук
    void play();

    /// Приостановить звук
    void pause();

    /// Остановить звук
    void stop();


private:
    // Можно продолжать работу с файлом
    bool canDo_; ///< Флаг допуска к работе с файлом

    // Можно играть звук
    bool canPlay_; ///< Флаг допуска к воспроизведению звука

    // Имя звука
    QString soundName_; ///< Имя файла

    // Последняя ошибка
    QString lastError_; ///< Текс последней ошибки

    // Переменная для хранения файла
    QFile* file_; ///< Контейнер файла

    // Информация о файле .wav
    wave_info_t wave_info_; ///< Структура информации о файле

    // Хранилище для data секции (самой музыки) файла .wav
    unsigned char* wavData_; ///< Контейнер секции data файла wav

    // Буфер OpenAL
    ALuint  buffer_; ///< Буфер OpenAL

    // Источник OpenAL
    ALuint  source_; ///< Источник OpenAL

    // Формат аудио (mono8/16 - stereo8/16) OpenAL
    ALenum  format_; ///< Формат аудио (mono8/16 - stereo8/16) OpenAL

    // Громкость
    int sourceVolume_; ///< Громкость

    // Скорости воспроизведения
    ALfloat sourcePitch_; ///< Скорость воспроизведения

    // Флаг зацикливания
    bool sourceLoop_; ///< Флаг зацикливания

    // Положение источника
    ALfloat sourcePosition_[3]; ///< Положение источника

    // "Скорость передвижения" источника
    ALfloat sourceVelocity_[3]; ///< "Скорость передвижения" источника

    /// Полная подготовка файла
    void loadSound_(QString soundname);

    /// Загрузка файла (в т.ч. из ресурсов)
    void loadFile_(QString soundname);

    /// Чтение информации о файле .wav
    void readWaveInfo_();

    /// Определение формата аудио (mono8/16 - stereo8/16)
    void defineFormat_();

    /// Генерация буфера и источника
    void generateStuff_();

    /// Настройка источника
    void configureSource_();

    /// Метод проверки необходимых параметров
    void checkValue(std::string baseStr, const char targStr[], QString err);
};




//-----------------------------------------------------------------------------
//
//      Класс управления очередью запуска звуков
//      (c) РГУПС, ВЖД 17/08/2017
//      Разработал: Ковшиков С. В.
//
//-----------------------------------------------------------------------------
/*!
 *  \file
 *  \brief Класс управления очередью запуска звуков
 *  \copyright РУГПС, ВЖД
 *  \author Ковшиков С. В.
 *  \date 17/08/2017
 */

/*!
 * \brief Класс управления очередью запуска звуков
 * \class ASoundController
 */
class ASOUNDSHARED_EXPORT ASoundController : public QObject
{
    Q_OBJECT

public:
    /// Конструктор
    ASoundController(QObject* parent = Q_NULLPTR);
    /// Деструктор
    ~ASoundController();

    /// Установить звук запуска
    void setSoundBegin(QString soundPath);

    /// Добавить звук процесса работы
    void appendSoundRunning(QString soundPath);

    /// Установить список звуков процесса работы
    void setSoundsRunning(QStringList soundPaths);

    /// Установить звук остановки
    void setSoundEnd(QString soundPath);


public slots:
    /// Запустить алгоритм воспроизведения (запуск устройства)
    void begin();

    /// Установить звук процесса работы
    void switchRunningSound(int index);

    /// Завершить алгоритм воспроизведения (остановка устройства)
    void end();

    /// Установить скорость воспроизведения
    void setPitch(float pitch);

    /// Установить громкость 0 - 100
    void setVolume(int volume);

    /// Аварийно завершить алгоритм вопсроизведения в любой момент
    void forcedStop();


private slots:
    /// Слот обработки таймера переключения звуков
    void onTimerSoundChanger();


private:
    /// Флаг готовности
    bool prepared_;

    /// Флаг запуска устройства
    bool beginning_;

    /// Флаг проигрывания
    bool running_;

    /// Индекс текущей фазы звука
    int currentSoundIndex_;

    /// Скорость воспроизведения
    float soundPitch_;

    /// Громкость воспроизведения
    int soundVolume_;

    /// Звук включения системы
    ASound* soundBegin_;

    /// Список фаз процесса работы
    QList<ASound*> listRunningSounds_;

    /// Звук выключения системы
    ASound* soundEnd_;

    /// Таймер перехода от включения к процессу работы
    QTimer* timerSoundChanger_;

    /// Проверить готовность всех звуков
    void prepare_();

    /// Очистить список фаз процесса работы
    void clearRunningSoundsList_();
};

#endif // ASOUND_H

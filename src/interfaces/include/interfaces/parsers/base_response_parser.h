#pragma once

#include <gen/error.h>
#include <gen/logclass.h>
#include <interfaces/types/common_types.h>
#include <s2/s2util.h>

namespace Interface
{

/// \brief Базовый класс парсера ответов от устройства.
class BaseResponseParser : public QObject
{
    Q_OBJECT
protected:
    QByteArray m_responseBuffer;
    QByteArray m_longDataBuffer;
    CommandStruct m_request;
    bool m_isFirstSectionReceived;
    bool m_isLastSectionReceived;
    bool m_isLastSectionSended;
    S2Util m_util;

    /// \brief Базовая функция для отправки наверх прочитанных из устройства файлов.
    void fileReceived(const QByteArray &file, const S2::FilesEnum addr, const DataTypes::FileFormat format);

public:
    explicit BaseResponseParser(QObject *parent = nullptr);

    /// \brief Установка последнего отправленного запроса
    /// для полученного текущего ответа от устройства.
    /// \details Может использоваться в дочерних классах
    void setRequest(const CommandStruct &request) noexcept;
    /// \brief Возвращает статус, была ли получена последняя
    /// секция данных при запросе на чтение файла.
    bool isLastSectionReceived() const noexcept;

    /// \brief Функция для добавления принятых от устройства данных
    /// в конец текущего буффера, содержащего ответ устройства.
    /// \details Применятся для защиты данных от фрагментирования.
    /// \see clearResponseBuffer.
    void accumulateToResponseBuffer(const QByteArray &responsePart) noexcept;
    /// \brief Функция для возврата сохранённого буфера с ответом от устройства.
    const QByteArray &getResponseBuffer() const noexcept;
    /// \brief Функция для очистки буффера, содержащего ответ устройства.
    /// \see accumulateToResponseBuffer.
    void clearResponseBuffer() noexcept;

    /// \brief Проверка размера ответа, полученного от устройства.
    /// \returns true - если ответ получен полностью, false - получен фрагментированный ответ.
    virtual bool isCompleteResponse() = 0;
    /// \brief Проверяет, является ли валидным ответ, полученный от устройства.
    virtual Error::Msg validate() = 0;
    /// \brief Функция парсинга полученного от устройства ответа на запрос.
    virtual void parse() = 0;

public slots:
    /// \brief Слот, вызываемый при получении от парсера запросов к
    /// устройству сигнала о том, что была отправлена последняя секция на запись.
    void lastSectionSended() noexcept;

    /// \brief Слот, отправляющий наверх размер читаемого/записываемого
    /// файла или большого массива байт.
    void processProgressRange(const quint64 count) noexcept;
    /// \brief Слот, отправляющий наверх прогресс чтения/записи
    /// файла или большого массива байт.
    void processProgressCount(const quint64 count) noexcept;

    /// \brief Слот, отправляющий наверх статус "OK", полученный от устройства.
    void processOk() noexcept;
    /// \brief Слот, отправляющий наверх код ошибки, полученный от устройства.
    void processError(int errorCode = 0) noexcept;

signals:
    /// \brief Сигнал, информирующий исполнителя на верхнем уровне
    /// о чтении файла или большого массива данных из устройства.
    void readingLongData();
    /// \brief Сигнал, который используется для отправки на вышестоящий уровень
    /// ответов от устройства, статусов, кодов ошибки, прогресса записи/чтения больших данных.
    void responseParsed(const Interface::DeviceResponse &response);

    /// \brief Сигнал, который используется для отправки сообщения в лог исполнителя запросов.
    void needToLog(const QString &message, const LogLevel level);
};

} // namespace Interface

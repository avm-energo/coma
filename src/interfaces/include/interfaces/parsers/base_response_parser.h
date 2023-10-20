#pragma once

#include <interfaces/types/common_types.h>
#include <s2/s2util.h>

namespace Interface
{

class BaseResponseParser : public QObject
{
    Q_OBJECT
protected:
    QByteArray m_response;
    CommandStruct m_request;
    bool m_isLastSectionReceived;
    S2Util m_util;

    void processProgressCount(const quint64 count);
    void processProgressRange(const quint64 count);
    void processOk();
    void processError(int errorCode = 0);

    void fileReceived(const QByteArray &ba, const S2::FilesEnum addr, const DataTypes::FileFormat format);

public:
    explicit BaseResponseParser(QObject *parent = nullptr);

    /// \brief Установка последнего отправленного запроса
    /// для полученного текущего ответа от устройства.
    /// \details Может использоваться в дочерних классах
    void setRequest(const CommandStruct &request) noexcept;
    /// \brief Возвращает статус, была ли получена последняя
    /// секция данных при запросе на чтение файла.
    bool isLastSectionReceived() const noexcept;

    /// \brief Проверяет, является ли валидным ответ, полученный от устройства.
    virtual bool isValid(const QByteArray &response) = 0;
    /// \brief Функция парсинга полученного от устройства ответа на запрос.
    virtual void parse(const QByteArray &response) = 0;

public slots:
    void totalBytes(const int total);
    void progressBytes(const int progress);

signals:
    void responseParsed(const Interface::DeviceResponse &response);
};

} // namespace Interface

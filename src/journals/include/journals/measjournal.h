#pragma once

#include <journals/basejournal.h>
#include <journals/measparser.h>

namespace journals
{

/// \brief Класс с данными журнало измерений.
class MeasJournal : public BaseJournal
{
private:
    MeasSettings m_settings;
    UniquePointer<MeasParser> m_parser;

    /// \brief Метод для получения названия столбцов модели из настроек описания модели.
    QStringList getHeaders();

    /// \brief Перегрузка виртуального метода для заполнения модели.
    /// \details Используется экземпляр класса MeasParser.
    /// \see MeasParser
    virtual void fillModel(const QByteArray &ba) override;

public:
    /// \brief Конструктор класса модели рабочего журнала.
    /// \param settings[in] - список настроек для описания модели
    /// журнала с измерениями. Парсится из XML конфигурации устройства.
    explicit MeasJournal(const MeasSettings &settings, QObject *parent = nullptr);
};

}

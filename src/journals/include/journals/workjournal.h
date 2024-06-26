#pragma once

#include <journals/basejournal.h>
#include <journals/eventparser.h>

namespace journals
{

/// \brief Класс с данными рабочих журналов.
class WorkJournal : public BaseJournal
{
private:
    QMap<quint32, QString> m_desriptions;
    UniquePointer<EventParser> m_parser;

    /// \brief Перегрузка виртуального метода для заполнения модели.
    /// \details Используется экземпляр класса EventParser.
    /// \see EventParser
    virtual void fillModel(const QByteArray &ba) override;

public:
    /// \brief Конструктор класса модели рабочего журнала.
    /// \param desc[in] - хэш-карта с описанием событий рабочего
    /// журнала. Парсится из XML конфигурации устройства.
    explicit WorkJournal(const QMap<quint32, QString> &desc, QObject *parent = nullptr);
};

}

#pragma once

#include <journals/basejournal.h>
#include <journals/eventparser.h>

namespace journals
{

/// \brief Класс с данными системных журналов.
class SysJournal : public BaseJournal
{
private:
    static const QMap<quint32, QString> s_desriptions;
    UniquePointer<EventParser> m_parser;

    /// \brief Перегрузка виртуального метода для заполнения модели.
    /// \details Используется экземпляр класса EventParser.
    /// \see EventParser
    virtual void fillModel(const QByteArray &ba) override;

public:
    explicit SysJournal(QObject *parent = nullptr);
};

}

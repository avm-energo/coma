#pragma once

#include "basejournal.h"
#include "eventparser.h"

namespace journals
{

/// \brief Класс с данными системных журналов.
class SysJournal : public BaseJournal
{
private:
    static const QMap<quint32, QString> desriptions;
    UniquePointer<EventParser> parser;

    /// \brief Перегрузка виртуального метода для заполнения модели.
    /// \details Используется экземпляр класса EventParser.
    /// \see EventParser
    virtual void fillModel(const QByteArray &ba) override;

public:
    explicit SysJournal(QObject *parent = nullptr);
};

}

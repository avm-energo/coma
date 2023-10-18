#pragma once

#include <deque>
#include <interfaces/types/common_types.h>

namespace Interface
{

class BaseRequestParser : public QObject
{
    Q_OBJECT
protected:
    static const std::map<Commands, CommandRegisters> s_wsCmdMap;

    QByteArray m_command;
    std::deque<QByteArray> m_writeLongData;
    bool m_isExceptionalSituation;

    void setExceptionalSituationStatus(bool status) noexcept;

public:
    explicit BaseRequestParser(QObject *parent = nullptr);

    virtual QByteArray parse(const CommandStruct &command) = 0;
    bool isExceptionalSituation() const noexcept;

signals:
    void totalBytes(const int total);
    void progressBytes(const int progress);
    void parsedCommand(const Interface::CommandStruct &command);
};

} // namespace Interface

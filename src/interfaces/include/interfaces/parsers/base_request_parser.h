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

    QByteArray m_request;
    std::deque<QByteArray> m_writeLongData;
    bool m_isExceptionalSituation;

    void setExceptionalSituationStatus(bool status) noexcept;

public:
    explicit BaseRequestParser(QObject *parent = nullptr);

    virtual QByteArray parse(const CommandStruct &command) = 0;

    bool isExceptionalSituation() const noexcept;
    virtual void exceptionalAction(const CommandStruct &command) noexcept;

    QByteArray getNextChunk();

signals:
    void writingFile();
    void readingFile();
    void totalBytes(const int total);
    void progressBytes(const int progress);
    void parsedCommand(const Interface::CommandStruct &command);
};

} // namespace Interface

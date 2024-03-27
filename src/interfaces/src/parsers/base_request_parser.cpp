#include "interfaces/parsers/base_request_parser.h"

namespace Interface
{

const std::map<Commands, CommandRegisters> BaseRequestParser::s_wsCmdMap {
    { Commands::C_StartWorkingChannel, StartWorkingChannel },     //
    { Commands::C_EraseJournals, EraseSystemJournal },            //
    { Commands::C_SetStartupValues, SetStartupValues },           //
    { Commands::C_SetStartupPhaseA, SetStartupPhaseA },           //
    { Commands::C_SetStartupPhaseB, SetStartupPhaseB },           //
    { Commands::C_SetStartupPhaseC, SetStartupPhaseC },           //
    { Commands::C_SetStartupUnbounced, SetStartupUnbounced },     //
    { Commands::C_SetTransOff, SetTransOff },                     //
    { Commands::C_ClearStartupValues, ClearStartupValues },       //
    { Commands::C_ClearStartupUnbounced, ClearStartupUnbounced }, //
    { Commands::C_ClearStartupError, ClearStartupSetError }       //
};

BaseRequestParser::BaseRequestParser(QObject *parent)
    : QObject(parent), m_isExceptionalSituation(false), m_progressCount(0)
{
}

void BaseRequestParser::updateProtocolSettings(const ProtocolDescription &desc) noexcept
{
    m_protocol.merge(desc);
}

QByteArray BaseRequestParser::getNextDataSection()
{
    if (m_longDataSections.size() > 0)
    {
        QByteArray nextChunk { m_longDataSections.front() };
        m_longDataSections.pop_front();
        if (m_longDataSections.size() == 0)
            emit writingLastSection();
        m_progressCount += nextChunk.size();
        emit progressBytes(m_progressCount);
        return nextChunk;
    }
    return QByteArray {};
}

void BaseRequestParser::setExceptionalSituationStatus(bool status) noexcept
{
    m_isExceptionalSituation = status;
}

bool BaseRequestParser::isExceptionalSituation() const noexcept
{
    return m_isExceptionalSituation;
}

void BaseRequestParser::exceptionalAction(const CommandStruct &command) noexcept
{
    // Nothing to do here
    Q_UNUSED(command);
    setExceptionalSituationStatus(false);
}

} // namespace Interface

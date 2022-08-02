#include "comaexception.h"

ComaException::ComaException(const QString &msg) : QException()
{
    m_message = msg;
}

ComaException::ComaException() : QException()
{
    m_message = "Unknown exception was thrown";
}

QString ComaException::message()
{
    return m_message;
}

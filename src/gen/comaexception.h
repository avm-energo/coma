#ifndef COMAEXCEPTION_H
#define COMAEXCEPTION_H

#include <QException>
#include <QMap>
#include <QObject>

class ComaException : public QException
{

public:
    ComaException(const QString &msg);
    ComaException();

    void raise() const override
    {
        throw *this;
    }
    ComaException *clone() const override
    {
        return new ComaException(*this);
    }

    QString message();

private:
    QString m_message;
};

#endif // COMAEXCEPTION_H

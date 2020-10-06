#ifndef WARN_H
#define WARN_H

#include <QBitArray>
#include <QObject>

class Warn : public QObject
{
    Q_OBJECT
public:
    explicit Warn(QObject *parent = nullptr);

signals:

private:
    int m_warnBdNum;
    int m_startWarnAddress;
    QBitArray m_warnFlags; // '1' equals warning
};

#endif // WARN_H

#ifndef WARN_H
#define WARN_H

#include <QObject>

class Warn : public QObject
{
    Q_OBJECT
public:
    explicit Warn(QObject *parent = nullptr);

signals:

};

#endif // WARN_H

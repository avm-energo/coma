#ifndef OSCILLOGRAM_H
#define OSCILLOGRAM_H

#include "../module/modules.h"

#include <QObject>

class Oscillogram : public QObject
{
    Q_OBJECT
public:
    explicit Oscillogram(QObject *parent = nullptr);

signals:

private:
};

#endif // OSCILLOGRAM_H

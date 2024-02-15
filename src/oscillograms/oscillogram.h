#ifndef OSCILLOGRAM_H
#define OSCILLOGRAM_H

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

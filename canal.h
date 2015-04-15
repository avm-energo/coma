#ifndef CANAL_H
#define CANAL_H

#include <QObject>

class canal : public QObject
{
    Q_OBJECT
public:
    explicit canal(QObject *parent = 0);
    ~canal();

signals:

public slots:
};

#endif // CANAL_H

#ifndef WAITTHREAD_H
#define WAITTHREAD_H

#include <QObject>

class WaitThread : public QObject
{
    Q_OBJECT

public:
    explicit WaitThread(QObject *parent = 0);

signals:
    void TenMsPassed();

public slots:
    void Run();
    void Stop();

private:
    bool StopRequest;
};

#endif // WAITTHREAD_H

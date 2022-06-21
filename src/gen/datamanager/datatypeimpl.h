#ifndef DATATYPEIMPL_H
#define DATATYPEIMPL_H

#include <QObject>

template <typename T> class DataTypeImpl
{
private:
    T *ptr;

public:
    DataTypeImpl(T *derived) : ptr(derived)
    {
    }

    void SendSignal()
    {
        emit DataTypeReceived(*ptr);
    }
};

#endif // DATATYPEIMPL_H

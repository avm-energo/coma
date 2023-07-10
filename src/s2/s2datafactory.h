#pragma once

#include <QObject>

namespace S2
{

class DataFactory : public QObject
{
    Q_OBJECT
public:
    explicit DataFactory(QObject *parent = nullptr);
};

}

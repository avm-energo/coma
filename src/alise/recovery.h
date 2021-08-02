#ifndef RECOVERY_H
#define RECOVERY_H

#include "../gen/datatypes.h"

class Recovery : public QObject
{
    Q_OBJECT
public:
    Recovery(QObject *parent = nullptr);

    void receiveBlock(const DataTypes::BlockStruct blk);
signals:
    void rebootReq();

private:
    void eth0();
    void eth2();
    void sync();
    void restartNetwork();
    bool resetInit = false;
};
#endif // RECOVERY_H

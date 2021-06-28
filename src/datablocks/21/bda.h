#ifndef BDA21_H
#define BDA21_H

#include "../datablock.h"

class Bda : public DataBlock
{
    Q_OBJECT
public:
    struct BlockData
    {
        float sin[8];
    };

    Bda(QObject *parent = nullptr);
    void createWidget() override;
    void updateWidget() override;

private:
    BlockData *m_blockData;
    QWidget *m_widget;
};

#endif // BDA21_H

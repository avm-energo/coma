#pragma once

#include <datablocks/datablock.h>

class Bac82 : public DataBlock
{
    Q_OBJECT
public:
    struct BlockData
    {                   // регулировочные коэффициенты
        float KmU[6];   // по напряжению
        float KmI_5[6]; // по току в пределе 5 А
        float KmI_1[6]; // по току в пределе 1 А
        float DPsi[6];  // по углу
        float K_freq;   // по частоте
        float Kinter;   // коэффициент взаимного влияния каналов
    };

    Bac82(QObject *parent = nullptr);

    void setDefBlock(Bac82::BlockData &blck);
    void setupValuesDesc() override;
    BlockData *data();
    void setDefBlock() override;

private:
    std::unique_ptr<BlockData> m_blockData;
};

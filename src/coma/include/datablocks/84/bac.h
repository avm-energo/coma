#pragma once

#include <datablocks/datablock.h>

class Bac84 : public DataBlock
{
    Q_OBJECT
public:
    struct BlockData
    { // Структура калибровочных параметров
        // записываются во флеш базовой платы
        float K_freq; // коррекция частоты
    };

    Bac84(QObject *parent = nullptr);

    void setupValuesDesc() override;
    BlockData *data();
    void setDefBlock() override;

private:
    std::unique_ptr<BlockData> m_blockData;
};

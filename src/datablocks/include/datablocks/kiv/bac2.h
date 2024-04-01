#pragma once

#include <datablocks/datablock.h>

class Bac2A284 : public DataBlock
{
    Q_OBJECT
public:
    struct BlockData
    { // Структура калибровочных параметров
        //записываются во флеш базовой платы
        float K_freq; // коррекция частоты
        float Art;    // коэффициент в канале Pt100, ед.АЦП/Ом
        float Brt;    // смещение в канале Pt100, ед.АЦП
    };

    Bac2A284(QObject *parent = nullptr);

    void setupValuesDesc() override;
    BlockData *data();
    void setDefBlock() override;

private:
    std::unique_ptr<BlockData> m_blockData;
};

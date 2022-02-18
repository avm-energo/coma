#ifndef KIV_BDAIN_H
#define KIV_BDAIN_H

#include "../datablock.h"

class BdaIn : public DataBlock
{
    Q_OBJECT
public:
    struct BlockData
    { // Структура блока выходных данных
        // в масштабах входных сигналов (для настройки)
        float Frequency;       // Частота в сети, Гц
        float IUefNat_filt[6]; // Истинные действующие значения сигналов (в вольтах или мА на входе)
        float IUeff_filtered[6]; // действующие значения сигналов по 1-й гармонике
        float phi_next_f[6]; // Углы сдвига сигналов по 1-й гармонике относительно Ua в градусах
        float Cbush[3]; // емкости вводов
        float Tg_d[3];  // tg delta вводов
        float Pt100_R;  // Измеренное сопротивление термометра, Ом
    };

    BdaIn(QObject *parent = nullptr);

    void createWidget() override;
    void setDefBlock() override;
    void updateWidget() override;
    BlockData *data();

private:
    std::unique_ptr<BlockData> m_blockData;
    //    QWidget *m_widget;
};

#endif // KIV_BDAIN_H

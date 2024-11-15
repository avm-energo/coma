#pragma once

#include <datablocks/datablock.h>

class Bac21 : public DataBlock
{
    Q_OBJECT
private:
    struct ChReg1
    {
        float fbin_I; ///< Калибровочные коэффициенты смещения по току в 8 каналах
        float fkuin; ///< Калибровочные коэффициенты наклона по напряжению в 8 каналах
        float fkiin; ///< Калибровочные коэффициенты наклона по току в 8 каналах
    };
    struct ChReg2
    {
        float fbin_U; ///< Калибровочные коэффициенты смещения по напряжению в 8 каналах
    };

    /// \brief Структура калибровочных параметров записываются во флеш мезонинной платы:
    struct BlockData
    {
        ChReg1 bac1[8];
        ChReg2 bac2[8];
    };

    /// \brief Структура калибровочных коэффициентов для выдачи на экран
    struct BlockDataForWidget
    {
        float *fkiin[8];
        float *fbin_I[8];
        float *fkuin[8];
        float *fbin_U[8];
    };

    std::unique_ptr<BlockData> m_blockData;
    BlockDataForWidget m_blockDataW;

public:
    Bac21(u8 numblock, QObject *parent = nullptr);
    void setupValuesDesc() override;
    BlockData *data();
    void setDefBlock() override;
};

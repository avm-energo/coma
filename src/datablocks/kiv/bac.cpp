#include "bac.h"

#include "../../gen/colors.h"
#include "../../gen/stdfunc.h"
#include "../../widgets/wd_func.h"

#include <QGroupBox>
#include <QVBoxLayout>

BacA284::BacA284(QObject *parent) : DataBlock(parent)
{
    m_blockData = std::unique_ptr<BlockData>(new BlockData);
    setBlock({ 2, "Настроечные параметры", DataTypes::DataBlockTypes::BacBlock, m_blockData.get(), sizeof(BlockData) });
    //    createWidget();
    readBlockFromModule();
    //    updateWidget();
}

void BacA284::setupValuesDesc()
{
    addNewGroup("Число витков первичной обмотки", "N1_TT", 0, 3, &m_blockData->N1_TT[0], 5);
    addNewGroup("Коэффициенты по напряжению", "KmU", 3, 3, &m_blockData->KmU[0], 5);
    addNewGroup("Коэффициенты по току для Кацп=1", "KmI1", 6, 3, &m_blockData->KmI1[0], 5);
    addNewGroup("Коэффициенты по току для Кацп=2", "KmI2", 9, 3, &m_blockData->KmI2[0], 5);
    addNewGroup("Коэффициенты по току для Кацп=4", "KmI4", 12, 3, &m_blockData->KmI4[0], 5);
    addNewGroup("Коэффициенты по току для Кацп=8", "KmI8", 15, 3, &m_blockData->KmI8[0], 5);

    addNewGroup("Коэффициенты по току для Кацп=16", "KmI16", 18, 3, &m_blockData->KmI16[0], 5);

    addNewGroup("Коэффициенты по току для Кацп=32", "KmI32", 21, 3, &m_blockData->KmI32[0], 5);

    addNewGroup("Коэффициенты линейной коррекции по tg delta", "TKPsi_a", 24, 3, &m_blockData->TKPsi_a[0], 5);
    addNewGroup("Коэффициенты квадратичной коррекции по tg delta", "TKPsi_b", 27, 3, &m_blockData->TKPsi_b[0], 5);
    addNewGroup("Коррекция фазы в градусах", "DPsi", 30, 6, &m_blockData->DPsi[0], 5);

    addNewGroup("Коэффициенты линейной коррекции по U и I", "TKUa", 36, 6, &m_blockData->TKUa[0], 5);

    addNewGroup("Коэффициенты квадратичной коррекции по U и I", "TKUb", 42, 6, &m_blockData->TKUb[0], 5);
    addNewValue("K_freq", "Коррекция частоты", &m_blockData->K_freq);
    addNewValue("Art", "Коэффициент в канале Pt100", &m_blockData->Art);
    addNewValue("Brt", "Смещение в канале Pt100", &m_blockData->Brt);
    addNewValue("Tmk0", "Начальная температура МК для коррекции", &m_blockData->Tmk0);
}

void BacA284::setDefBlock()
{
    m_blockData->Art = 81.5f;
    m_blockData->Brt = 6033.0f;
    m_blockData->Tmk0 = 24;
    m_blockData->K_freq = 1;

    for (int i = 0; i < 6; i++)
    {
        m_blockData->DPsi[i] = 0;
        m_blockData->TKUa[i] = 0;
        m_blockData->TKUb[i] = 0;
    }

    for (int i = 0; i < 3; i++)
    {
        m_blockData->KmU[i] = 1;
        m_blockData->KmI1[i] = 1;
        m_blockData->KmI16[i] = 1;
        m_blockData->KmI2[i] = 1;
        m_blockData->KmI32[i] = 1;
        m_blockData->KmI4[i] = 1;
        m_blockData->KmI8[i] = 1;
        m_blockData->N1_TT[i] = 10;
        m_blockData->TKPsi_a[i] = 0;
        m_blockData->TKPsi_b[i] = 0;
    }
}

BacA284::BlockData *BacA284::data()
{
    return m_blockData.get();
}

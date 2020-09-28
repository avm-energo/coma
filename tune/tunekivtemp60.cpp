#include "tunekivtemp60.h"
#include "../gen/colors.h"
#include "../gen/stdfunc.h"
#include "../usb/eprotocom.h"
#include <QMessageBox>
#include <QVBoxLayout>

TuneKIVTemp60::TuneKIVTemp60(QWidget *parent) : EAbstractTuneDialog(parent)
{
    m_tuneStep = 2;
    if (LoadTuneSequenceFile() != Error::Msg::NoError)
        return;
    //    AddBac(&m_Bac_block, M_BACBLOCKNUM, sizeof(m_Bac_block));
}

void TuneKIVTemp60::SetupUI()
{
    QString tmps = "QDialog {background-color: " + QString(Colors::ACONFCLR) + ";}";
    setStyleSheet(tmps);
    QWidget *cp1 = new QWidget; // = TuneUI();
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(TuneUI());
    cp1->setLayout(lyout);
    QTabWidget *TuneTW = new QTabWidget;
    TuneTW->addTab(cp1, "Протокол наладки");
}

void TuneKIVTemp60::SetLbls()
{
    lbls.append("1. Ввод пароля...");
    lbls.append("2. Сохранение текущей конфигурации...");
    lbls.append("7.2.1. Задание максимальных токов 600 мА...");
    lbls.append("7.2.2. Отображение схемы подключения...");
    lbls.append("7.2.7. Получение результатов измерений...");

    lbls.append("5. Установка коэффициентов...");
    lbls.append("7. 7_3_2. Получение текущих аналоговых данных...");
    lbls.append("8. 7.3.4. Информация...");
    lbls.append(
        "9. 7.3.4.2. Конфигуратор рассчитывает новые значения калибровочных коэффициентов по току для Кацп=1...");
    lbls.append("10. 7.3.4.6 Переход на новую конфигурацию 400мА...");
    lbls.append(
        "11. 7.3.4.7. Конфигуратор рассчитывает новые значения калибровочных коэффициентов по току для Кацп=2...");
    lbls.append("12. 7.3.4.11. Переход на новую конфигурацию 200мА......");
    lbls.append(
        "13. 7.3.4.12. Конфигуратор рассчитывает новые значения калибровочных коэффициентов по току для Кацп=4...");
    lbls.append("14. 7.3.4.13. Переход на новую конфигурацию 100мА......");
    lbls.append(
        "15. 7.3.4.14. Конфигуратор рассчитывает новые значения калибровочных коэффициентов по току для Кацп=8...");
    lbls.append("16. 7.3.4.15. Переход на новую конфигурацию 50мА......");
    lbls.append(
        "17. 7.3.4.16. Конфигуратор рассчитывает новые значения калибровочных коэффициентов по току для Кацп=16...");
    lbls.append("18. 7.3.4.17. Переход на новую конфигурацию 25мА......");
    lbls.append(
        "19. 7.3.4.18. Конфигуратор рассчитывает новые значения калибровочных коэффициентов по току для Кацп=32...");
    lbls.append("20. 7.3.5. Восстановление сохранённой конфигурации и проверка...");
}

void TuneKIVTemp60::SetPf()
{
    int count = 0;
    pf[lbls.at(count++)] = &EAbstractTuneDialog::CheckPassword;
    int (EAbstractTuneDialog::*func)()
        = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKIV::SaveWorkConfig);
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKIV::Start7_2_1);
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKIV::ShowScheme);
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKIV::Start_7_2_7);
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKIV::Start7_3_1); // 4. Информация
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(
        &TuneKIVTemp60::SetNewTuneCoefs); // 5. Установка коэффициентов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(
        &TuneKIVTemp60::Start7_3_2); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKIV::Start7_3_4); // Информация
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKIV::Start7_3_4_2); // Расчёт коррекции по фазе
    pf[lbls.at(count++)] = func;
    func
        = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKIV::Start7_3_4_6); // Расчёт коррекции по частоте
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(
        &TuneKIVTemp60::Start7_3_4_7); // Конфигуратор рассчитывает новые значения калибровочных коэффициентов по току
    // для Кацп=2
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(
        &TuneKIVTemp60::Start7_3_4_11); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(
        &TuneKIVTemp60::Start7_3_4_12); // Расчёт коррекции взаимного влияния каналов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(
        &TuneKIVTemp60::Start7_3_4_13); // Получение текущих аналоговых данных и расчёт настроечных коэффициентов по
    // напряжениям
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKIV::Start7_3_4_14); // Сохранение конфигурации
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(
        &TuneKIVTemp60::Start7_3_4_15); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKIV::Start7_3_4_16); // Ввод измеренных значений
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(
        &TuneKIVTemp60::Start7_3_4_17); // Расчёт настроечных коэффициентов по токам, напряжениям и углам
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogKIV::Start7_3_4_18); // Сохранение конфигурации
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(
        &TuneKIVTemp60::Start7_3_5); // Восстановление сохранённой конфигурации и проверка
    pf[lbls.at(count++)] = func;
}

void TuneKIVTemp60::FillBac(int bacnum)
{
    Q_UNUSED(bacnum);
    for (int i = 0; i < 3; i++)
    {
        WDFunc::SetLEData(this, "tune" + QString::number(i), QString::number(m_Bac_block.N1_TT[i], 'g', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 3), QString::number(m_Bac_block.KmU[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 6), QString::number(m_Bac_block.KmI1[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 9), QString::number(m_Bac_block.KmI2[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 12), QString::number(m_Bac_block.KmI4[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 15), QString::number(m_Bac_block.KmI8[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 18), QString::number(m_Bac_block.KmI16[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 21), QString::number(m_Bac_block.KmI32[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 24), QString::number(m_Bac_block.TKPsi_a[i], 'e', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 27), QString::number(m_Bac_block.TKPsi_b[i], 'e', 5));
    }

    for (int i = 0; i < 6; i++)
    {
        WDFunc::SetLEData(this, "tune" + QString::number(i + 30), QString::number(m_Bac_block.DPsi[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 36), QString::number(m_Bac_block.TKUa[i], 'e', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 42), QString::number(m_Bac_block.TKUb[i], 'e', 5));
    }

    WDFunc::SetLEData(this, "tune48", QString::number(m_Bac_block.K_freq, 'f', 5));
    WDFunc::SetLEData(this, "tune49", QString::number(m_Bac_block.Art, 'f', 5));
    WDFunc::SetLEData(this, "tune50", QString::number(m_Bac_block.Brt, 'f', 5));
    WDFunc::SetLEData(this, "tune51", QString::number(m_Bac_block.Tmk0, 'f', 5));
}

void TuneKIVTemp60::FillNewBac()
{
    for (int i = 0; i < 3; i++)
    {
        WDFunc::SetLEData(this, "tune" + QString::number(i), QString::number(m_Bac_newblock.N1_TT[i], 'g', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 3), QString::number(m_Bac_newblock.KmU[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 6), QString::number(m_Bac_newblock.KmI1[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 9), QString::number(m_Bac_newblock.KmI2[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 12), QString::number(m_Bac_newblock.KmI4[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 15), QString::number(m_Bac_newblock.KmI8[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 18), QString::number(m_Bac_newblock.KmI16[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 21), QString::number(m_Bac_newblock.KmI32[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 24), QString::number(m_Bac_newblock.TKPsi_a[i], 'e', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 27), QString::number(m_Bac_newblock.TKPsi_b[i], 'e', 5));
    }

    for (int i = 0; i < 6; i++)
    {
        WDFunc::SetLEData(this, "tune" + QString::number(i + 30), QString::number(m_Bac_newblock.DPsi[i], 'f', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 36), QString::number(m_Bac_newblock.TKUa[i], 'e', 5));
        WDFunc::SetLEData(this, "tune" + QString::number(i + 42), QString::number(m_Bac_newblock.TKUb[i], 'e', 5));
    }

    WDFunc::SetLEData(this, "tune48", QString::number(m_Bac_newblock.K_freq, 'f', 5));
    WDFunc::SetLEData(this, "tune49", QString::number(m_Bac_newblock.Art, 'f', 5));
    WDFunc::SetLEData(this, "tune50", QString::number(m_Bac_newblock.Brt, 'f', 5));
    WDFunc::SetLEData(this, "tune51", QString::number(m_Bac_newblock.Tmk0, 'f', 5));
}

void TuneKIVTemp60::FillBackBac(int bacnum)
{
    Q_UNUSED(bacnum);
    QString tmps;
    for (int i = 0; i < 3; i++)
    {
        WDFunc::LE_read_data(this, "tune" + QString::number(i), tmps);
        m_Bac_block.N1_TT[i] = tmps.toUInt();
        WDFunc::LE_read_data(this, "tune" + QString::number(i + 3), tmps);
        m_Bac_block.KmU[i] = ToFloat(tmps);
        WDFunc::LE_read_data(this, "tune" + QString::number(i + 6), tmps);
        m_Bac_block.KmI1[i] = ToFloat(tmps);
        WDFunc::LE_read_data(this, "tune" + QString::number(i + 9), tmps);
        m_Bac_block.KmI2[i] = ToFloat(tmps);
        WDFunc::LE_read_data(this, "tune" + QString::number(i + 12), tmps);
        m_Bac_block.KmI4[i] = ToFloat(tmps);
        WDFunc::LE_read_data(this, "tune" + QString::number(i + 15), tmps);
        m_Bac_block.KmI8[i] = ToFloat(tmps);
        WDFunc::LE_read_data(this, "tune" + QString::number(i + 18), tmps);
        m_Bac_block.KmI16[i] = ToFloat(tmps);
        WDFunc::LE_read_data(this, "tune" + QString::number(i + 21), tmps);
        m_Bac_block.KmI32[i] = ToFloat(tmps);
        WDFunc::LE_read_data(this, "tune" + QString::number(i + 24), tmps);
        m_Bac_block.TKPsi_a[i] = ToFloat(tmps);
        WDFunc::LE_read_data(this, "tune" + QString::number(i + 27), tmps);
        m_Bac_block.TKPsi_b[i] = ToFloat(tmps);
    }

    for (int i = 0; i < 6; i++)
    {
        WDFunc::LE_read_data(this, "tune" + QString::number(i + 30), tmps);
        m_Bac_block.DPsi[i] = ToFloat(tmps);
        WDFunc::LE_read_data(this, "tune" + QString::number(i + 36), tmps);
        m_Bac_block.TKUa[i] = ToFloat(tmps);
        WDFunc::LE_read_data(this, "tune" + QString::number(i + 42), tmps);
        m_Bac_block.TKUb[i] = ToFloat(tmps);
    }

    WDFunc::LE_read_data(this, "tune48", tmps);
    m_Bac_block.K_freq = ToFloat(tmps);
    WDFunc::LE_read_data(this, "tune49", tmps);
    m_Bac_block.Art = ToFloat(tmps);
    WDFunc::LE_read_data(this, "tune50", tmps);
    m_Bac_block.Brt = ToFloat(tmps);
    WDFunc::LE_read_data(this, "tune51", tmps);
    m_Bac_block.Tmk0 = ToFloat(tmps);
}

Error::Msg TuneKIVTemp60::LoadTuneSequenceFile() { return CheckCalibrStep(); }

void TuneKIVTemp60::SetDefCoefs()
{
    m_Bac_block.Art = 44.65f;
    m_Bac_block.Brt = 3345.75f;
    m_Bac_block.Tmk0 = 24;
    m_Bac_block.K_freq = 1;

    for (int i = 0; i < 6; i++)
    {
        m_Bac_block.DPsi[i] = 0;
        m_Bac_block.TKUa[i] = 0;
        m_Bac_block.TKUb[i] = 0;
    }

    for (int i = 0; i < 3; i++)
    {
        m_Bac_block.KmU[i] = 1;
        m_Bac_block.KmI1[i] = 1;
        m_Bac_block.KmI16[i] = 1;
        m_Bac_block.KmI2[i] = 1;
        m_Bac_block.KmI32[i] = 1;
        m_Bac_block.KmI8[i] = 1;
        m_Bac_block.N1_TT[i] = 10;
        m_Bac_block.TKPsi_a[i] = 0;
        m_Bac_block.TKPsi_b[i] = 0;
    }
    m_Bac_block.KmI4[0] = 0.997060814f;
    m_Bac_block.KmI4[1] = 0.993458061f;
    m_Bac_block.KmI4[2] = 0.992731500f;

    m_VModel->updateModel();
    //    FillBac(BoardTypes::BT_BASE);
}

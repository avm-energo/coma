#include "confkivdialog.h"

#include "../gen/board.h"
#include "../gen/error.h"
#include "../gen/s2.h"
#include "../widgets/wd_func.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QTimer>
#include <QVBoxLayout>

ConfKIVDialog::ConfKIVDialog(ConfigKIV *ckiv, QWidget *parent) : AbstractConfDialog(parent)
{
    CKIV = ckiv;
}

ConfKIVDialog::~ConfKIVDialog()
{
    delete CKIV;
}

void ConfKIVDialog::Fill()
{
    using namespace DataTypes;
    CKIV->MainConfig()->Fill();
    CKIV->KxxConfig()->Fill();
    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.Unom1), S2::getRecord(BciNumber::Unom1).value<float>());
    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.Umin), S2::getRecord(BciNumber::DUImin_ID).value<float>());
    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.Imin), S2::getRecord(BciNumber::Imin).value<float>());
    // WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.Unom), CKIV->Bci_block.Unom);
    //  WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.Umin), CKIV->Bci_block.Umin);
    //  WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.Imin), CKIV->Bci_block.Imin);

    const auto tg_pasp = S2::getRecord(BciNumber::Tg_Pasp_ID).value<FLOAT_3t>();
    for (auto i = 0; i != tg_pasp.size(); ++i)
        WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.Tg_pasp) + QString::number(i), tg_pasp.at(i));
    const auto c_pasp = S2::getRecord(BciNumber::C_Pasp_ID).value<FLOAT_3t>();
    for (auto i = 0; i != c_pasp.size(); ++i)
        WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.C_pasp) + QString::number(i), c_pasp.at(i));
    // for (int i = 0; i < 3; i++)
    //  {

    // WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.Tg_pasp) + QString::number(i), CKIV->Bci_block.Tg_pasp[i]);
    //   WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.C_pasp) + QString::number(i), CKIV->Bci_block.C_pasp[i]);
    //}

    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.dC_pred), S2::getRecord(BciNumber::dC_pred).value<float>());
    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.dC_alarm), S2::getRecord(BciNumber::dC_alarm).value<float>());
    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.dTg_pred), S2::getRecord(BciNumber::dTg_pred).value<float>());
    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.dTg_alarm), S2::getRecord(BciNumber::dTg_alarm).value<float>());
    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.dIunb_pred), S2::getRecord(BciNumber::dIunb_pred).value<float>());
    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.dIunb_alarm), S2::getRecord(BciNumber::dIunb_alarm).value<float>());
    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.GdC), S2::getRecord(BciNumber::GdC).value<float>());
    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.GdTg), S2::getRecord(BciNumber::GdTg).value<float>());
    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.GdIunb), S2::getRecord(BciNumber::GdIunb).value<float>());
    //    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.dC_pred), CKIV->Bci_block.dC_pred);
    //    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.dC_alarm), CKIV->Bci_block.dC_alarm);
    //    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.dTg_pred), CKIV->Bci_block.dTg_pred);
    //    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.dTg_alarm), CKIV->Bci_block.dTg_alarm);
    //    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.dIunb_pred), CKIV->Bci_block.dIunb_pred);
    //    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.dIunb_alarm), CKIV->Bci_block.dIunb_alarm);
    //    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.GdC), CKIV->Bci_block.GdC);
    //    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.GdTg), CKIV->Bci_block.GdTg);
    //    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.GdIunb), CKIV->Bci_block.GdIunb);

    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.NFiltr), S2::getRecord(BciNumber::NFiltr_ID).value<DWORD>());
    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.T_Data_Rec), S2::getRecord(BciNumber::T_Data_Rec).value<DWORD>());
    // WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.NFiltr), CKIV->Bci_block.NFiltr);
    // WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.T_Data_Rec), CKIV->Bci_block.T_Data_Rec);

    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.U2nom), S2::getRecord(BciNumber::U2nom).value<float>());
    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.LowU), S2::getRecord(BciNumber::LowU).value<float>());
    // WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.U2nom), CKIV->Bci_block.U2nom);
    // WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.LowU), CKIV->Bci_block.LowU);

    WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.Tevent_pred), S2::getRecord(BciNumber::Tevent_pred).value<DWORD>());
    WDFunc::SetSPBData(
        this, NAMEOF(CKIV->Bci_block.Tevent_alarm), S2::getRecord(BciNumber::Tevent_alarm).value<DWORD>());
    //   WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.Tevent_pred), CKIV->Bci_block.Tevent_pred);
    // WDFunc::SetSPBData(this, NAMEOF(CKIV->Bci_block.Tevent_alarm), CKIV->Bci_block.Tevent_alarm);

    WDFunc::SetChBData(this, NAMEOF(CKIV->Bci_block.IsU), bool(S2::getRecord(BciNumber::IsU).value<BYTE>()));
    WDFunc::SetChBData(this, NAMEOF(CKIV->Bci_block.IsIunb), bool(S2::getRecord(BciNumber::IsIuIunb).value<BYTE>()));
    //   WDFunc::SetChBData(this, NAMEOF(CKIV->Bci_block.IsU), bool(CKIV->Bci_block.IsU));
    //  WDFunc::SetChBData(this, NAMEOF(CKIV->Bci_block.IsIunb), bool(CKIV->Bci_block.IsIunb));
    FillKiv();
}

void ConfKIVDialog::FillBack()
{
    using namespace DataTypes;
    CKIV->MainConfig()->FillBack();
    CKIV->KxxConfig()->FillBack();

    S2::setRecordValue({ BciNumber::Unom1, WDFunc::SPBData<float>(this, NAMEOF(CKIV->Bci_block.Unom1)) });
    S2::setRecordValue({ BciNumber::DUImin_ID, WDFunc::SPBData<float>(this, NAMEOF(CKIV->Bci_block.Umin)) });
    S2::setRecordValue({ BciNumber::Imin, WDFunc::SPBData<float>(this, NAMEOF(CKIV->Bci_block.Imin)) });
    //    WDFunc::SPBData(this, NAMEOF(CKIV->Bci_block.Unom1), CKIV->Bci_block.Unom1);
    //    WDFunc::SPBData(this, NAMEOF(CKIV->Bci_block.Umin), CKIV->Bci_block.Umin);
    //    WDFunc::SPBData(this, NAMEOF(CKIV->Bci_block.Imin), CKIV->Bci_block.Imin);

    FLOAT_3t tg_pasp;
    for (auto i = 0; i != tg_pasp.size(); ++i)
        tg_pasp.at(i) = WDFunc::SPBData<float>(this, NAMEOF(CKIV->Bci_block.Tg_pasp) + QString::number(i));
    S2::setRecordValue({ BciNumber::Tg_Pasp_ID, tg_pasp });
    // for (int i = 0; i < 3; i++)
    // {
    // WDFunc::SPBData(this, NAMEOF(CKIV->Bci_block.Tg_pasp) + QString::number(i), CKIV->Bci_block.Tg_pasp[i]);
    // WDFunc::SPBData(this, NAMEOF(CKIV->Bci_block.C_pasp) + QString::number(i), CKIV->Bci_block.C_pasp[i]);
    //}
    FLOAT_3t c_pasp;
    for (auto i = 0; i != c_pasp.size(); ++i)
        c_pasp.at(i) = WDFunc::SPBData<float>(this, NAMEOF(CKIV->Bci_block.C_pasp) + QString::number(i));
    S2::setRecordValue({ BciNumber::C_Pasp_ID, c_pasp });

    S2::setRecordValue({ BciNumber::dC_pred, WDFunc::SPBData<float>(this, NAMEOF(CKIV->Bci_block.dC_pred)) });
    S2::setRecordValue({ BciNumber::dC_alarm, WDFunc::SPBData<float>(this, NAMEOF(CKIV->Bci_block.dC_alarm)) });
    S2::setRecordValue({ BciNumber::dTg_pred, WDFunc::SPBData<float>(this, NAMEOF(CKIV->Bci_block.dTg_pred)) });
    S2::setRecordValue({ BciNumber::dTg_alarm, WDFunc::SPBData<float>(this, NAMEOF(CKIV->Bci_block.dTg_alarm)) });
    S2::setRecordValue({ BciNumber::dIunb_pred, WDFunc::SPBData<float>(this, NAMEOF(CKIV->Bci_block.dIunb_pred)) });
    S2::setRecordValue({ BciNumber::dIunb_alarm, WDFunc::SPBData<float>(this, NAMEOF(CKIV->Bci_block.dIunb_alarm)) });
    //    WDFunc::SPBData(this, NAMEOF(CKIV->Bci_block.dC_pred), CKIV->Bci_block.dC_pred);
    //    WDFunc::SPBData(this, NAMEOF(CKIV->Bci_block.dC_alarm), CKIV->Bci_block.dC_alarm);
    //    WDFunc::SPBData(this, NAMEOF(CKIV->Bci_block.dTg_pred), CKIV->Bci_block.dTg_pred);
    //    WDFunc::SPBData(this, NAMEOF(CKIV->Bci_block.dTg_alarm), CKIV->Bci_block.dTg_alarm);
    //    WDFunc::SPBData(this, NAMEOF(CKIV->Bci_block.dIunb_pred), CKIV->Bci_block.dIunb_pred);
    //    WDFunc::SPBData(this, NAMEOF(CKIV->Bci_block.dIunb_alarm), CKIV->Bci_block.dIunb_alarm);
    //    WDFunc::SPBData(this, NAMEOF(CKIV->Bci_block.GdC), CKIV->Bci_block.GdC);
    //    WDFunc::SPBData(this, NAMEOF(CKIV->Bci_block.GdTg), CKIV->Bci_block.GdTg);
    //    WDFunc::SPBData(this, NAMEOF(CKIV->Bci_block.GdIunb), CKIV->Bci_block.GdIunb);

    S2::setRecordValue({ BciNumber::NFiltr_ID, WDFunc::SPBData<DWORD>(this, NAMEOF(CKIV->Bci_block.NFiltr)) });
    S2::setRecordValue({ BciNumber::T_Data_Rec, WDFunc::SPBData<DWORD>(this, NAMEOF(CKIV->Bci_block.T_Data_Rec)) });
    // WDFunc::SPBData(this, NAMEOF(CKIV->Bci_block.NFiltr), CKIV->Bci_block.NFiltr);
    //  WDFunc::SPBData(this, NAMEOF(CKIV->Bci_block.T_Data_Rec), CKIV->Bci_block.T_Data_Rec);

    S2::setRecordValue({ BciNumber::U2nom, WDFunc::SPBData<float>(this, NAMEOF(CKIV->Bci_block.U2nom)) });
    S2::setRecordValue({ BciNumber::LowU, WDFunc::SPBData<float>(this, NAMEOF(CKIV->Bci_block.LowU)) });
    //   WDFunc::SPBData(this, NAMEOF(CKIV->Bci_block.U2nom), CKIV->Bci_block.U2nom);
    //  WDFunc::SPBData(this, NAMEOF(CKIV->Bci_block.LowU), CKIV->Bci_block.LowU);

    S2::setRecordValue({ BciNumber::Tevent_pred, WDFunc::SPBData<DWORD>(this, NAMEOF(CKIV->Bci_block.Tevent_pred)) });
    S2::setRecordValue({ BciNumber::Tevent_alarm, WDFunc::SPBData<DWORD>(this, NAMEOF(CKIV->Bci_block.Tevent_alarm)) });
    //  WDFunc::SPBData(this, NAMEOF(CKIV->Bci_block.Tevent_pred), CKIV->Bci_block.Tevent_pred);
    // WDFunc::SPBData(this, NAMEOF(CKIV->Bci_block.Tevent_alarm), CKIV->Bci_block.Tevent_alarm);

    S2::setRecordValue({ BciNumber::IsU, WORD(WDFunc::ChBData(this, NAMEOF(CKIV->Bci_block.IsU))) });
    S2::setRecordValue({ BciNumber::IsIuIunb, WORD(WDFunc::ChBData(this, NAMEOF(CKIV->Bci_block.IsIunb))) });
    //    WDFunc::ChBData(this, NAMEOF(CKIV->Bci_block.IsU), Variable);
    //    if (Variable)
    //        CKIV->Bci_block.IsU = 1;
    //    else
    //        CKIV->Bci_block.IsU = 0;

    //    WDFunc::ChBData(this, NAMEOF(CKIV->Bci_block.IsIunb), Variable);
    //    if (Variable)
    //        CKIV->Bci_block.IsIunb = 1;
    //    else
    //        CKIV->Bci_block.IsIunb = 0;
    FillBackKiv();
}

QWidget *ConfKIVDialog::analogWidget()
{
    QWidget *w = new QWidget;

    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *gridlyout = new QGridLayout;
    gridlyout->setAlignment(Qt::AlignVCenter);

    const QStringList phase { "Фаза A:", "Фаза B:", "Фаза C:" };

    int row = 0;
    QGroupBox *gb = new QGroupBox("Аналоговые параметры");

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальное линейное первичное напряжение, кВ:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, NAMEOF(CKIV->Bci_block.Unom1), 0, 10000, 0), row, 2, 1, 3);
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Номинальное вторичное напряжение первой тройки, В:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, NAMEOF(CKIV->Bci_block.U2nom), 0, 10000, 1), row, 2, 1, 3);
    row++;

    for (int i = 0; i < phase.size(); i++)
    {
        gridlyout->addWidget(WDFunc::NewLBL2(this, phase.at(i)), row, 2 + i, 1, 1, Qt::AlignTop);
    }
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Паспортные значения ёмкости вводов, пФ:"), row, 1, 1, 1, Qt::AlignTop);
    for (int i = 0; i < 3; i++)
    {

        gridlyout->addWidget(WDFunc::NewSPB2(this, NAMEOF(CKIV->Bci_block.C_pasp) + QString::number(i), 0, 20000, 1),
            row, 2 + i, 1, 1, Qt::AlignTop);
    }
    row++;

    gridlyout->addWidget(WDFunc::NewLBL2(this, "Паспортные значения tg вводов, %:"), row, 1, 1, 1);

    for (int i = 0; i < 3; i++)
    {
        gridlyout->addWidget(
            WDFunc::NewSPB2(this, NAMEOF(CKIV->Bci_block.Tg_pasp) + QString::number(i), 0, 10, 2), row, 2 + i, 1, 1);
    }
    row++;

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);

    w->setLayout(lyout);
    return w;
}

QWidget *ConfKIVDialog::thresholdsWidget()
{
    QWidget *w = new QWidget;

    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *gridlyout = new QGridLayout;

    int row = 0;
    QGroupBox *gb = new QGroupBox("Уставки сигнализации");

    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка предупредительной сигнализации по изменению емкости, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, NAMEOF(CKIV->Bci_block.dC_pred), 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка аварийной сигнализации по изменению емкости, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, NAMEOF(CKIV->Bci_block.dC_alarm), 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка предупредительной сигнализации по изменению tg δ, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, NAMEOF(CKIV->Bci_block.dTg_pred), 0, 1000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Уставка аварийной сигнализации по изменению tg δ, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, NAMEOF(CKIV->Bci_block.dTg_alarm), 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this,
                             "Уставка предупредительной сигнализации по "
                             "изменению небаланса токов, %:"),
        row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, NAMEOF(CKIV->Bci_block.dIunb_pred), 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка аварийной сигнализации по изменению небаланса токов, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, NAMEOF(CKIV->Bci_block.dIunb_alarm), 0, 10000, 1), row, 2, 1, 3);

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Уставки контроля минимума тока и напряжения");
    gridlyout = new QGridLayout;

    row = 0;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Уставка контроля минимума напряжения (в % от номинального):"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, NAMEOF(CKIV->Bci_block.Umin), 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Уставка контроля минимума тока (в % от Imax):"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, NAMEOF(CKIV->Bci_block.Imin), 0, 10000, 1), row, 2, 1, 3);

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);
    w->setLayout(lyout);
    return w;
}

QWidget *ConfKIVDialog::remainsWidget()
{
    QWidget *w = new QWidget;
    QGroupBox *gb = new QGroupBox("Гистерезис");

    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *gridlyout = new QGridLayout;
    int row = 0;

    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Гистерезис на отключение сигнализации по dC, % от уставки:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, NAMEOF(CKIV->Bci_block.GdC), 0, 10000, 1), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Гистерезис на отключение сигнализации по dTg, % от уставки:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, NAMEOF(CKIV->Bci_block.GdTg), 0, 10000, 3), row, 2, 1, 3);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Гистерезис на отключение сигнализации по небалансу токов, %:"), row, 1, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, NAMEOF(CKIV->Bci_block.GdIunb), 0, 10000, 1), row, 2, 1, 3);
    gb->setLayout(gridlyout);
    lyout->addWidget(gb);

    //............................................................

    gb = new QGroupBox("Сигнализации событий");
    gridlyout = new QGridLayout;

    row = 0;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Низкое напряжение для сигнализации:"), row, 0, 1, 1, Qt::AlignLeft);
    gridlyout->addWidget(WDFunc::NewSPB2(this, NAMEOF(CKIV->Bci_block.LowU), 0, 10000, 1), row, 1, 1, 1);

    gridlyout->addWidget(
        WDFunc::NewChB2(this, NAMEOF(CKIV->Bci_block.IsU), "Сигнализация по наличию входного напряжения, % от ном."),
        row, 2, 1, 1);

    row++;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Задержка на формирование предупредительных событий, сек:"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, NAMEOF(CKIV->Bci_block.Tevent_pred), 0, 10000, 1), row, 1, 1, 1);

    gridlyout->addWidget(
        WDFunc::NewChB2(this, NAMEOF(CKIV->Bci_block.IsIunb), "Сигнализация по небалансу токов"), row, 2, 1, 1);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Задержка на формирование аварийных событий, сек:"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, NAMEOF(CKIV->Bci_block.Tevent_alarm), 0, 10000, 1), row, 1, 1, 1);

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("");
    gridlyout = new QGridLayout;

    row = 0;
    gridlyout->addWidget(
        WDFunc::NewLBL2(this, "Интервал усреднения данных  (в периодах основной частоты):"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, NAMEOF(CKIV->Bci_block.NFiltr), 0, 10000, 0), row, 1, 1, 3);

    row++;
    gridlyout->addWidget(WDFunc::NewLBL2(this, "Интервал записи данных в ПЗУ (тренд), в секундах:"), row, 0, 1, 1);
    gridlyout->addWidget(WDFunc::NewSPB2(this, NAMEOF(CKIV->Bci_block.T_Data_Rec), 0, 10000, 0), row, 1, 1, 3);

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);
    w->setLayout(lyout);
    return w;
}

QWidget *ConfKIVDialog::connectionWidget()
{
    QWidget *w = new QWidget;

    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox;

    QGridLayout *gridlyout = new QGridLayout;

    gb->setTitle("Настройки протокола МЭК-60870-5-104");

    gridlyout->addWidget(CKIV->MainConfig()->MainWidget(this), 0, 0, 1, 1);
    gridlyout->addWidget(CKIV->KxxConfig()->ComParam(this), 0, 1, 1, 1);

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Настройка времени");

    vlyout->addWidget(CKIV->MainConfig()->TimeWidget(this));

    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    w->setLayout(lyout);
    return w;
}

void ConfKIVDialog::FillKiv()
{
    WDFunc::SetSPBData(this, "Unom", CKIV->Bci_block.Unom1);
    WDFunc::SetSPBData(this, "Umin", CKIV->Bci_block.Umin);
    WDFunc::SetSPBData(this, "Imin", CKIV->Bci_block.Imin);

    for (int i = 0; i < 3; i++)
    {

        WDFunc::SetSPBData(this, "Tg_pasp." + QString::number(i), CKIV->Bci_block.Tg_pasp[i]);
        WDFunc::SetSPBData(this, "C_pasp." + QString::number(i), CKIV->Bci_block.C_pasp[i]);
    }

    WDFunc::SetSPBData(this, "dС_pred", CKIV->Bci_block.dC_pred);
    WDFunc::SetSPBData(this, "dС_alarm", CKIV->Bci_block.dC_alarm);
    WDFunc::SetSPBData(this, "dTg_pred", CKIV->Bci_block.dTg_pred);
    WDFunc::SetSPBData(this, "dTg_alarm", CKIV->Bci_block.dTg_alarm);
    WDFunc::SetSPBData(this, "dIunb_pred", CKIV->Bci_block.dIunb_pred);
    WDFunc::SetSPBData(this, "dIunb_alarm", CKIV->Bci_block.dIunb_alarm);
    WDFunc::SetSPBData(this, "GdС", CKIV->Bci_block.GdC);
    WDFunc::SetSPBData(this, "GdTg", CKIV->Bci_block.GdTg);
    WDFunc::SetSPBData(this, "GdIunb", CKIV->Bci_block.GdIunb);
    WDFunc::SetSPBData(this, "NFiltr", CKIV->Bci_block.NFiltr);
    WDFunc::SetSPBData(this, "T_Data_Rec", CKIV->Bci_block.T_Data_Rec);
    WDFunc::SetSPBData(this, "U2nom", CKIV->Bci_block.U2nom);
    WDFunc::SetSPBData(this, "Ulow", CKIV->Bci_block.LowU);
    WDFunc::SetSPBData(this, "Tevent_pred", CKIV->Bci_block.Tevent_pred);
    WDFunc::SetSPBData(this, "Tevent_alarm", CKIV->Bci_block.Tevent_alarm);

    WDFunc::SetSPBData(this, "IsU", bool(CKIV->Bci_block.IsU));
    WDFunc::SetSPBData(this, "IsIunb", bool(CKIV->Bci_block.IsIunb));
}

void ConfKIVDialog::FillBackKiv()
{
    WDFunc::SPBData(this, "Unom", CKIV->Bci_block.Unom1);
    WDFunc::SPBData(this, "Umin", CKIV->Bci_block.Umin);
    WDFunc::SPBData(this, "Imin", CKIV->Bci_block.Imin);

    for (int i = 0; i < 3; i++)
    {

        WDFunc::SPBData(this, "Tg_pasp." + QString::number(i), CKIV->Bci_block.Tg_pasp[i]);
        WDFunc::SPBData(this, "C_pasp." + QString::number(i), CKIV->Bci_block.C_pasp[i]);
    }

    WDFunc::SPBData(this, "dС_pred", CKIV->Bci_block.dC_pred);
    WDFunc::SPBData(this, "dС_alarm", CKIV->Bci_block.dC_alarm);
    WDFunc::SPBData(this, "dTg_pred", CKIV->Bci_block.dTg_pred);
    WDFunc::SPBData(this, "dTg_alarm", CKIV->Bci_block.dTg_alarm);
    WDFunc::SPBData(this, "dIunb_pred", CKIV->Bci_block.dIunb_pred);
    WDFunc::SPBData(this, "dIunb_alarm", CKIV->Bci_block.dIunb_alarm);
    WDFunc::SPBData(this, "GdС", CKIV->Bci_block.GdC);
    WDFunc::SPBData(this, "GdTg", CKIV->Bci_block.GdTg);
    WDFunc::SPBData(this, "GdIunb", CKIV->Bci_block.GdIunb);
    WDFunc::SPBData(this, "NFiltr", CKIV->Bci_block.NFiltr);
    WDFunc::SPBData(this, "T_Data_Rec", CKIV->Bci_block.T_Data_Rec);
    WDFunc::SPBData(this, "U2nom", CKIV->Bci_block.U2nom);
    WDFunc::SPBData(this, "Ulow", CKIV->Bci_block.LowU);
    WDFunc::SPBData(this, "Tevent_pred", CKIV->Bci_block.Tevent_pred);
    WDFunc::SPBData(this, "Tevent_alarm", CKIV->Bci_block.Tevent_alarm);

    WDFunc::ChBData(this, "IsU", CKIV->Bci_block.IsU);
    WDFunc::ChBData(this, "IsU", CKIV->Bci_block.IsIunb);
}

void ConfKIVDialog::SetupUI()
{

    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");

    ConfTW->addTab(analogWidget(), "Аналоговые");

    ConfTW->addTab(thresholdsWidget(), "Уставки");

    ConfTW->addTab(remainsWidget(), "Остальное");

    ConfTW->addTab(connectionWidget(), "Связь");

    ConfTW->addTab(CKIV->KxxConfig()->ModbusWidget(this), "ModBusMaster");
    ConfTW->addTab(CKIV->KxxConfig()->VariousWidget(this), "Общее");

    lyout->addWidget(ConfTW);
    lyout->addWidget(ConfButtons());
    setLayout(lyout);
}

void ConfKIVDialog::CheckConf()
{
}

void ConfKIVDialog::SetDefConf()
{
    CKIV->setDefConf();

    Fill();
}

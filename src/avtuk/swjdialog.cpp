#include "swjdialog.h"

#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/modules.h"
#include "../gen/timefunc.h"
#include "../widgets/wd_func.h"
#include "eoscillogram.h"
#include "trendviewdialog.h"

SWJDialog::SWJDialog(EOscillogram *osc, int mode, QWidget *parent) : QDialog(parent)
{
    Mode = mode;

    SWJOscFunc = osc;
}

void SWJDialog::Init(S2DataTypes::SwitchJourInfo swj)
{
    SWJInf = swj;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    QStringList phase { "фазы А, В, С", "фаза А", "фаза В", "фаза С" };
    float value;
    QString str, tmps;

    GetSwjOscData();

    glyout->addWidget(WDFunc::NewLBL2(this, "Номер"), 0, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "Дата, время"), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "Аппарат"), 0, 2, 1, 2);
    glyout->addWidget(WDFunc::NewLBL2(this, "Переключение"), 0, 4, 1, 2);
    glyout->addWidget(WDFunc::NewLBLT2(this, QString::number(SWJOscFunc->SWJRecord.num)), 1, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT2(this, TimeFunc::UnixTime64ToString(SWJOscFunc->SWJRecord.time)), 1, 1, 1, 1);
    const QStringList tmpsl { "CB", "G", "D" };
    if (SWJOscFunc->SWJRecord.typeA == 1)
        tmps = tmpsl.at(0); //: "N/A";
    else if (SWJOscFunc->SWJRecord.typeA == 2)
        tmps = tmpsl.at(1);
    else if (SWJOscFunc->SWJRecord.typeA == 4)
        tmps = tmpsl.at(2);
    else
        tmps = "N/A";
    glyout->addWidget(WDFunc::NewLBLT2(this, tmps), 1, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT2(this, QString::number(SWJOscFunc->SWJRecord.numA)), 1, 3, 1, 1);
    tmps = (SWJOscFunc->SWJRecord.options & 0x00000001) ? "ВКЛЮЧЕНИЕ" : "ОТКЛЮЧЕНИЕ";
    glyout->addWidget(WDFunc::NewLBLT2(this, tmps), 1, 4, 1, 2);
    glyout->addWidget(WDFunc::NewLBL2(this, "Тип коммутации:"), 3, 0, 1, 4);
    if ((SWJOscFunc->SWJRecord.options >> 1))
    {
        if (((SWJOscFunc->SWJRecord.options >> 1) & 0x00000001))
            tmps = "Несинхронная от АВМ-СК";

        if (((SWJOscFunc->SWJRecord.options >> 1) & 0x00000011) == 3)
            tmps = "Синхронная от АВМ-СК";
    }
    else
    {
        tmps = "Несинхронная от внешнего устройства";
    }
    glyout->addWidget(WDFunc::NewLBLT2(this, tmps), 3, 4, 1, 1);

    glyout->addWidget(WDFunc::NewLBL2(this, "Результат переключения:"), 4, 0, 1, 4);
    tmps = (SWJOscFunc->SWJRecord.result) ? "НЕУСПЕШНО" : "УСПЕШНО";
    glyout->addWidget(WDFunc::NewLBLT2(this, tmps), 4, 4, 1, 1);

    glyout->addWidget(WDFunc::NewLBL2(this, "Коммутируемые фазы:"), 5, 0, 1, 4);
    for (int i = 0; i < 4; i++)
    {
        if (((SWJOscFunc->SWJRecord.options >> 3) == i))
        {
            tmps = phase.at(i);
        }
    }
    glyout->addWidget(WDFunc::NewLBLT2(this, tmps), 5, 4, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "Напряжение питания цепей соленоидов, В:"), 6, 0, 1, 4);

    if (SWJOscFunc->SWJRecord.supplyVoltage == std::numeric_limits<float>::max())
        glyout->addWidget(WDFunc::NewLBLT2(this, "-"), 6, 4, 1, 1);
    else
        glyout->addWidget(WDFunc::NewLBLT2(this, QString::number(SWJOscFunc->SWJRecord.supplyVoltage)), 6, 4, 1, 1);

    glyout->addWidget(WDFunc::NewLBL2(this, "Температура окружающей среды, Град:"), 7, 0, 1, 4);

    if (SWJOscFunc->SWJRecord.tOutside == std::numeric_limits<float>::max())
        glyout->addWidget(WDFunc::NewLBLT2(this, "-"), 7, 4, 1, 1);
    else
        glyout->addWidget(WDFunc::NewLBLT2(this, QString::number(SWJOscFunc->SWJRecord.tOutside)), 7, 4, 1, 1);

    if (SWJInf.fileLength)
    {
        glyout->addWidget(WDFunc::NewLBL2(this, "Осциллограмма:"), 8, 0, 1, 4);
        QPushButton *pb = new QPushButton("Открыть осциллограмму");
        pb->setIcon(QIcon(":/icons/osc.svg"));
        connect(pb, SIGNAL(clicked()), this, SLOT(ShowOsc()));
        glyout->addWidget(pb, 8, 4, 1, 1);
    }
    else
    {
        QPixmap *pm = new QPixmap(":/icons/hr.png");
        glyout->addWidget(WDFunc::NewLBL(this, "", "", "", pm), 8, 4, 1, 1);
    }
    vlyout->addLayout(glyout);
    vlyout->addStretch(10);
    glyout = new QGridLayout;
    const QStringList sl {
        "Действующее значение тока в момент коммутации, А",        //
        "Действующее значение напряжения в момент коммутации, кВ", //
        "Собственное время коммутации, мс",                        //
        "Полное время коммутации, мс",                             //
        "Время перемещения главного контакта, мс",                 //
        "Время горения дуги, мс",                                  //
        "Время безоперационного простоя к моменту коммутации, ч",  //
        "Погрешность синхронной коммутации, мс",                   //
        "Температура внутри привода, Град",                        //
        "Давление в гидросистеме привода, Па"                      //
    };
    glyout->addWidget(WDFunc::NewLBL2(this, "Измеренное значение"), 0, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "A"), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "B"), 0, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBL2(this, "C"), 0, 3, 1, 1);
    glyout->setColumnStretch(0, 10);
    int row = 1;
    glyout->addWidget(WDFunc::NewLBL2(this, sl.at(row - 1)), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
        glyout->addWidget(
            WDFunc::NewLBLT2(this, QString::number(SWJOscFunc->SWJRecord.amperage[i], 'f', 1)), row, i + 1, 1, 1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL2(this, sl.at(row - 1)), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
        glyout->addWidget(
            WDFunc::NewLBLT2(this, QString::number(SWJOscFunc->SWJRecord.voltage[i], 'f', 1)), row, i + 1, 1, 1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL2(this, sl.at(row - 1)), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
    {
        value = static_cast<float>(SWJOscFunc->SWJRecord.ownTime[i]);
        value = value / 100;
        glyout->addWidget(WDFunc::NewLBLT2(this, str.setNum(value, 'f', 2)), row, i + 1, 1, 1);
    }
    ++row;
    glyout->addWidget(WDFunc::NewLBL2(this, sl.at(row - 1)), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
    {
        value = static_cast<float>(SWJOscFunc->SWJRecord.fullTime[i]);
        value = value / 100;
        glyout->addWidget(WDFunc::NewLBLT2(this, str.setNum(value, 'f', 2)), row, i + 1, 1, 1);
    }
    ++row;
    glyout->addWidget(WDFunc::NewLBL2(this, sl.at(row - 1)), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
    {
        value = static_cast<float>(SWJOscFunc->SWJRecord.movTime[i]);
        value = value / 100;
        glyout->addWidget(WDFunc::NewLBLT2(this, str.setNum(value, 'f', 2)), row, i + 1, 1, 1);
    }
    ++row;
    glyout->addWidget(WDFunc::NewLBL2(this, sl.at(row - 1)), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
    {
        value = static_cast<float>(SWJOscFunc->SWJRecord.archTime[i]);
        value = value / 100;
        glyout->addWidget(WDFunc::NewLBLT2(this, str.setNum(value, 'f', 2)), row, i + 1, 1, 1);
    }
    ++row;
    glyout->addWidget(WDFunc::NewLBL2(this, sl.at(row - 1)), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
    {
        value = static_cast<float>(SWJOscFunc->SWJRecord.idleTime[i]);
        value = value / 100;
        glyout->addWidget(WDFunc::NewLBLT2(this, str.setNum(value, 'f', 2)), row, i + 1, 1, 1);
    }
    ++row;
    glyout->addWidget(WDFunc::NewLBL2(this, sl.at(row - 1)), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
    {
        value = static_cast<float>(SWJOscFunc->SWJRecord.inaccuracy[i]);
        value = value / 100;
        glyout->addWidget(WDFunc::NewLBLT2(this, str.setNum(value, 'f', 2)), row, i + 1, 1, 1);
    }
    ++row;
    glyout->addWidget(WDFunc::NewLBL2(this, sl.at(row - 1)), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
    {
        value = SWJOscFunc->SWJRecord.tInside[i];
        if (value == std::numeric_limits<float>::max())
            glyout->addWidget(WDFunc::NewLBLT2(this, "-"), row, i + 1, 1, 1);
        else
            glyout->addWidget(WDFunc::NewLBLT2(this, str.setNum(value, 'f', 2)), row, i + 1, 1, 1);
    }
    ++row;
    glyout->addWidget(WDFunc::NewLBL2(this, sl.at(row - 1)), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
    {
        value = SWJOscFunc->SWJRecord.phyd[i];
        if (value == std::numeric_limits<float>::max())
            glyout->addWidget(WDFunc::NewLBLT2(this, "-"), row, i + 1, 1, 1);
        else
            glyout->addWidget(WDFunc::NewLBLT2(this, str.setNum(value, 'f', 2)), row, i + 1, 1, 1);
    }
    /*++row;
    glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
    for (int i=0; i<3; ++i)
    {
        value = static_cast<float>(SWJOscFunc->SWJRecord.Inaccuracy[i]);
        value = value/100;
        glyout->addWidget(WDFunc::NewLBLT(this, str.setNum(value, 'f', 2)),row,i+1,1,1);
    }*/
    vlyout->addLayout(glyout);
    QPushButton *pb = new QPushButton("Сохранить журнал в файл");
    connect(pb, SIGNAL(clicked(bool)), this, SLOT(SaveSWJ()));
    vlyout->addWidget(pb);
    setLayout(vlyout);
}

void SWJDialog::LoadOsc(QByteArray &ba)
{
    // OscFunc->BA = ba;
    SWJOscFunc->BA.resize(ba.size());
    memcpy(&(SWJOscFunc->BA.data()[0]), &(ba.data()[0]), ba.size());
}

#if PROGSIZE != PROGSIZE_EMUL
void SWJDialog::SaveSWJ()
{
    QByteArray ba;
    int SWJSize = sizeof(SWJournalRecordStruct);
    int GBOSize = sizeof(EOscillogram::GBoStruct);
    ba.resize(SWJInf.FileLength);
    memcpy(&(ba.data()[0]), &SWJOscFunc->SWJRecord, SWJSize);
    // memcpy(&(ba.data()[SWJSize]), &SWJInf, GBOSize);
    if (Commands::GetOsc(SWJInf.FileNum, &(ba.data()[SWJSize + GBOSize])) == Error::ER_NOERROR)
        Files::SaveToFile(Files::ChooseFileForSave(this, "Файлы жуналов (*.swj)", "swj"), ba, ba.size());
    else
        EMessageBox::error(this, "Ошибка", "Ошибка чтения осциллограммы из модуля");
}

void SWJDialog::ShowOsc()
{
    dlg->PlotShow();
    dlg->show();
}
#endif

void SWJDialog::GetSwjOscData()
{
    QStringList tmpav, tmpdv;
    quint32 len = 0;
    // char *ptr;

#if PROGSIZE != PROGSIZE_EMUL
    if (Mode != SWJ_MODE_OFFLINE)
    {
        SWJOscFunc->BA.resize(SWJInf.FileLength + sizeof(S2::FileHeader));
        if (Commands::GetOsc(SWJInf.FileNum, &(SWJOscFunc->BA.data()[0])) != Error::ER_NOERROR)
        {
            WARNMSG("");
            return;
        }
        QString tmps = StdFunc::GetSystemHomeDir() + "/temporary.osc";
        Files::SaveToFile(tmps, SWJOscFunc->BA, SWJInf.FileLength);
    }
#endif
    m_trendViewDialog = new TrendViewDialog(SWJOscFunc->BA);
    std::unique_ptr<TrendViewModel> mdl
        = std::unique_ptr<TrendViewModel>(new TrendViewModel(QStringList(), QStringList(), len));
    SWJOscFunc->ProcessOsc(mdl.get());
    mdl->xmax = (static_cast<float>((mdl->Len) - 10));
    mdl->xmin = -10;

    switch (mdl->idOsc)
    {
    case AVTUK_85::OSC_ID:
    {

        m_trendViewDialog->setAnalogNames(mdl->tmpav_85);
        m_trendViewDialog->setDigitalNames(mdl->tmpdv_85);
        m_trendViewDialog->setDigitalColors(mdl->dcolors_85);
        m_trendViewDialog->setAnalogColors(mdl->acolors_85);
        m_trendViewDialog->setDiscreteDescriptions(mdl->ddescr_85);
        m_trendViewDialog->setAnalogDescriptions(mdl->adescr_85);
        m_trendViewDialog->setRange(mdl->xmin, mdl->xmax, -200, 200);
        break;
    }
    case AVTUK_8X::OSC_ID:
    {
        mdl->tmpdv_80.clear();
        m_trendViewDialog->setAnalogNames(mdl->tmpav_80);
        m_trendViewDialog->setDigitalNames(mdl->tmpdv_80);
        m_trendViewDialog->setDigitalColors(mdl->dcolors_80);
        m_trendViewDialog->setAnalogColors(mdl->acolors_80);
        m_trendViewDialog->setRange(mdl->xmin, mdl->xmax, -200, 200);
        break;
    }

    case AVTUK_21::OSC_ID_MIN:
    {
        // период отсчётов - 20 мс, длительность записи осциллограммы 10 сек, итого 500 точек по 4 байта на каждую
        mdl->tmpav_21 << QString::number(mdl->idOsc); // пока сделано для одного канала в осциллограмме
        // TrendViewModel *TModel = new TrendViewModel(QStringList(), tmpav, *len);
        // dlg->SetModel(TModel);
        m_trendViewDialog->setAnalogNames(mdl->tmpav_21);
        // 10000 мс, 20 мА (сделать автонастройку в зависимости от конфигурации по данному каналу)
        m_trendViewDialog->setRange(0, 10000, -20, 20);

        break;
    }

    case ID_OSC_CH0:
    case ID_OSC_CH0 + 1:
    case ID_OSC_CH0 + 2:
    case ID_OSC_CH0 + 3:
    case ID_OSC_CH0 + 4:
    case ID_OSC_CH0 + 5:
    case ID_OSC_CH0 + 6:
    case ID_OSC_CH0 + 7:
    {

        m_trendViewDialog->setAnalogNames(mdl->tmpav_85);
        m_trendViewDialog->setDigitalNames(mdl->tmpdv_85);
        m_trendViewDialog->setDigitalColors(mdl->dcolors_85);
        m_trendViewDialog->setAnalogColors(mdl->acolors_85);
        m_trendViewDialog->setRange(mdl->xmin, mdl->xmax, -200, 200);
        break;
    }
    }
    m_trendViewDialog->setTrendModel(std::move(mdl));
    m_trendViewDialog->setupPlots();
    m_trendViewDialog->setupUI();
    m_trendViewDialog->setModal(false);
}

TrendViewDialog *SWJDialog::trendViewDialog() const
{
    return m_trendViewDialog;
}

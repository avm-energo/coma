#include "oscmanager.h"

#include "../gen/datatypes.h"
#include "../gen/files.h"
#include "../gen/timefunc.h"
#include "../widgets/wd_func.h"
#include "eoscillogram.h"
#include "parseid9050.h"
#include "swjdialog.h"
#include "trendviewdialog.h"

OscManager::OscManager(QObject *parent) : QObject(parent)
{
}

void OscManager::LoadOscFromFile(const QString &filename)
{

    QByteArray buffer = 0;
    if (Files::LoadFromFile(filename, buffer) != Error::NoError)
        return;

    // TrendViewDialog *trendDlg = new TrendViewDialog();
    trendDialog = UniquePointer<TrendViewDialog>(new TrendViewDialog);
    trendDialog->setArrayToSave(buffer);
    EOscillogram *OscFunc = new EOscillogram(trendDialog.get());
    OscFunc->BA = buffer;
    std::unique_ptr<TrendViewModel> mdl = std::unique_ptr<TrendViewModel>(new TrendViewModel);
    OscFunc->ProcessOsc(mdl.get());
    mdl->xmax = (static_cast<float>(mdl->Len / 2));
    mdl->xmin = -mdl->xmax;

    switch (mdl->idOsc)
    {
    case MT_ID85:
    {

        trendDialog->setAnalogNames(mdl->tmpav_85);
        trendDialog->setDigitalNames(mdl->tmpdv_85);
        trendDialog->setDigitalColors(mdl->dcolors_85);
        trendDialog->setAnalogColors(mdl->acolors_85);
        trendDialog->setDiscreteDescriptions(mdl->ddescr_85);
        trendDialog->setAnalogDescriptions(mdl->adescr_85);
        trendDialog->setRange(mdl->xmin, mdl->xmax, -200, 200);
        break;
    }
    case MT_ID80:
    {
        mdl->tmpdv_80.clear();
        trendDialog->setAnalogNames(mdl->tmpav_80);
        trendDialog->setDigitalNames(mdl->tmpdv_80);
        trendDialog->setDigitalColors(mdl->dcolors_80);
        trendDialog->setAnalogColors(mdl->acolors_80);
        trendDialog->setRange(mdl->xmin, mdl->xmax, -200, 200);
        break;
    }

    case MT_ID21:
    {
        // период отсчётов - 20 мс, длительность записи осциллограммы 10 сек, итого 500 точек по 4 байта на каждую
        // mdl->tmpav_21 << QString::number(mdl->idOsc); // пока сделано для одного канала в осциллограмме
        // TrendViewModel *TModel = new TrendViewModel(QStringList(), tmpav, *len);
        // dlg->SetModel(TModel);
        trendDialog->setAnalogColors(mdl->acolors_21);
        trendDialog->setAnalogNames(mdl->tmpav_21);
        trendDialog->setAnalogDescriptions(mdl->adescr_21);
        // 10000 мс, 20 мА (сделать автонастройку в зависимости от конфигурации по данному каналу)
        trendDialog->setRange(0, 10000, -20, 20);

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

        trendDialog->setAnalogNames(mdl->tmpav_85);
        trendDialog->setDigitalNames(mdl->tmpdv_85);
        trendDialog->setDigitalColors(mdl->dcolors_85);
        trendDialog->setAnalogColors(mdl->acolors_85);
        trendDialog->setRange(mdl->xmin, mdl->xmax, -200, 200);
        break;
    }
    }
    trendDialog->setTrendModel(std::move(mdl));
    trendDialog->setupPlots();
    trendDialog->setupUI();
    trendDialog->showPlot();
    trendDialog->show();
}

void OscManager::loadSwjFromFile(const QString &filename)
{
    // bool haveosc;
    int SWJRSize = sizeof(S2DataTypes::SwitchJourRecord);
    qDebug() << SWJRSize << sizeof(S2DataTypes::SwitchJourInfo);
    // return;
    // int GBOSize = sizeof(EOscillogram::GBoStruct);

    QByteArray buffer = 0;

    if (Files::LoadFromFile(filename, buffer) != Error::NoError)
        return;

    if (buffer.size() < SWJRSize)
    {
        qCritical() << "Некорректная структура файла журнала";
        return;
    }
    QStringList phase = { "фазы А, В, С", "фаза А", "фаза В", "фаза С" };
    EOscillogram *OscFunc = new EOscillogram(this);
    OscFunc->BA = buffer;
    S2DataTypes::SwitchJourRecord SWJ;
    size_t tmpi = static_cast<size_t>(SWJRSize);
    memcpy(&SWJ, &(OscFunc->BA.data()[0]), tmpi); // копируем информацию о переключении
    S2DataTypes::SwitchJourInfo swjr;
    swjr.fileLength = OscFunc->BA.size();
    // EOscillogram::GBoStruct gbos;
    // tmpi = static_cast<size_t>(GBOSize);
    // memcpy(&gbos, &(ba.data()[SWJRSize]), tmpi); // копируем информацию об осциллограмме
    /* ba.remove(0, SWJRSize); // оставляем только саму осциллограмму
    if (ba.isEmpty()) // осциллограммы в журнале нет
        haveosc = false;
    else
        haveosc = true;*/
    // dlg->Init(swjr, haveosc, gbos);

    // if (!ba.isEmpty())
    //   dlg->LoadOsc(ba);

    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    SWJDialog *swjDialog = new SWJDialog(OscFunc, SWJDialog::SWJ_MODE_OFFLINE);
    swjDialog->GetSwjOscData();
    glyout->addWidget(new QLabel("Номер", swjDialog), 0, 0, 1, 1);
    glyout->addWidget(new QLabel("Дата, время", swjDialog), 0, 1, 1, 1);
    glyout->addWidget(new QLabel("Аппарат", swjDialog), 0, 2, 1, 2);
    glyout->addWidget(new QLabel("Переключение", swjDialog), 0, 4, 1, 2);
    glyout->addWidget(new QLabel(QString::number(OscFunc->SWJRecord.num), swjDialog), 1, 0, 1, 1);
    glyout->addWidget(new QLabel(TimeFunc::UnixTime64ToString(OscFunc->SWJRecord.time), swjDialog), 1, 1, 1, 1);
    QString str, tmps;
    QStringList tmpsl { "CB", "G", "D" };
    if (OscFunc->SWJRecord.typeA == 1)
        tmps = tmpsl.at(0); //: "N/A";
    else if (OscFunc->SWJRecord.typeA == 2)
        tmps = tmpsl.at(1);
    else if (OscFunc->SWJRecord.typeA == 4)
        tmps = tmpsl.at(2);
    else
        tmps = "N/A";
    glyout->addWidget(new QLabel(tmps, swjDialog), 1, 2, 1, 1);
    glyout->addWidget(new QLabel(QString::number(OscFunc->SWJRecord.numA), swjDialog), 1, 3, 1, 1);
    tmps = (OscFunc->SWJRecord.options & 0x00000001) ? "ВКЛЮЧЕНИЕ" : "ОТКЛЮЧЕНИЕ";
    glyout->addWidget(new QLabel(tmps, swjDialog), 1, 4, 1, 2);
    glyout->addWidget(new QLabel("Тип коммутации:", swjDialog), 3, 0, 1, 4);

    quint32 tmpi32 = (OscFunc->SWJRecord.options >> 1) & 0x03;
    switch (tmpi32)
    {
    case 2:
        tmps = "Несинхронная от АВМ-СК";
        break;
    case 3:
        tmps = "Синхронная от АВМ-СК";
        break;
    default:
        tmps = "Несинхронная от внешнего устройства";
        break;
    }

    glyout->addWidget(new QLabel(tmps, swjDialog), 3, 4, 1, 1);

    glyout->addWidget(new QLabel("Результат переключения:", swjDialog), 4, 0, 1, 4);
    tmps = (OscFunc->SWJRecord.result) ? "НЕУСПЕШНО" : "УСПЕШНО";
    glyout->addWidget(new QLabel(tmps, swjDialog), 4, 4, 1, 1);

    glyout->addWidget(new QLabel("Коммутируемые фазы:", swjDialog), 5, 0, 1, 4);
    for (int i = 0; i < 4; i++)
    {
        if (((OscFunc->SWJRecord.options >> 3) == i))
        {
            tmps = phase.at(i);
        }
    }
    glyout->addWidget(new QLabel(tmps, swjDialog), 5, 4, 1, 1);

    glyout->addWidget(new QLabel("Напряжение питания цепей соленоидов, В:", swjDialog), 6, 0, 1, 4);

    if (OscFunc->SWJRecord.supplyVoltage == std::numeric_limits<float>::max())
        glyout->addWidget(new QLabel("-", swjDialog), 6, 4, 1, 1);
    else
        glyout->addWidget(new QLabel(QString::number(OscFunc->SWJRecord.supplyVoltage), swjDialog), 6, 4, 1, 1);

    glyout->addWidget(new QLabel("Температура окружающей среды, Град:", swjDialog), 7, 0, 1, 4);

    if (OscFunc->SWJRecord.tOutside == std::numeric_limits<float>::max())
        glyout->addWidget(new QLabel("-", swjDialog), 7, 4, 1, 1);
    else
        glyout->addWidget(new QLabel(QString::number(OscFunc->SWJRecord.tOutside), swjDialog), 7, 4, 1, 1);

    if (swjr.fileLength)
    {
        glyout->addWidget(new QLabel("Осциллограмма:", swjDialog), 8, 0, 1, 4);
        QPushButton *pb = new QPushButton("Открыть осциллограмму", swjDialog);
        pb->setIcon(QIcon(":/icons/osc.svg"));
        connect(pb, &QPushButton::clicked, this, [swjDialog] {
            swjDialog->trendViewDialog()->showPlot();
            swjDialog->trendViewDialog()->show();
        });
        glyout->addWidget(pb, 8, 4, 1, 1);
    }
    else
    {
        QPixmap *pm = new QPixmap("images/hr.png");
        glyout->addWidget(WDFunc::NewLBL(swjDialog, "", "", "", pm), 8, 4, 1, 1);
    }
    vlyout->addLayout(glyout);
    vlyout->addStretch(10);
    glyout = new QGridLayout;
    QStringList sl {
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
    glyout->addWidget(new QLabel("Измеренное значение", swjDialog), 0, 0, 1, 1);
    glyout->addWidget(new QLabel("A", swjDialog), 0, 1, 1, 1);
    glyout->addWidget(new QLabel("B", swjDialog), 0, 2, 1, 1);
    glyout->addWidget(new QLabel("C", swjDialog), 0, 3, 1, 1);
    glyout->setColumnStretch(0, 10);
    int row = 1;
    glyout->addWidget(new QLabel(sl.at(row - 1), swjDialog), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
        glyout->addWidget(
            new QLabel(QString::number(OscFunc->SWJRecord.amperage[i], 'f', 1), swjDialog), row, i + 1, 1, 1);
    ++row;
    glyout->addWidget(new QLabel(sl.at(row - 1), swjDialog), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
        glyout->addWidget(
            new QLabel(QString::number(OscFunc->SWJRecord.voltage[i], 'f', 1), swjDialog), row, i + 1, 1, 1);
    ++row;
    glyout->addWidget(new QLabel(sl.at(row - 1), swjDialog), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
    {
        float value = static_cast<float>(OscFunc->SWJRecord.ownTime[i]);
        value = value / 100;
        glyout->addWidget(new QLabel(str.setNum(value, 'f', 2), swjDialog), row, i + 1, 1, 1);
    }
    ++row;
    glyout->addWidget(new QLabel(sl.at(row - 1), swjDialog), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
    {
        float value = static_cast<float>(OscFunc->SWJRecord.fullTime[i]);
        value = value / 100;
        glyout->addWidget(new QLabel(str.setNum(value, 'f', 2), swjDialog), row, i + 1, 1, 1);
    }
    ++row;
    glyout->addWidget(new QLabel(sl.at(row - 1), swjDialog), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
    {
        float value = static_cast<float>(OscFunc->SWJRecord.movTime[i]);
        value = value / 100;
        glyout->addWidget(new QLabel(str.setNum(value, 'f', 2), swjDialog), row, i + 1, 1, 1);
    }
    ++row;
    glyout->addWidget(new QLabel(sl.at(row - 1), swjDialog), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
    {
        float value = static_cast<float>(OscFunc->SWJRecord.archTime[i]);
        value = value / 100;
        glyout->addWidget(new QLabel(str.setNum(value, 'f', 2), swjDialog), row, i + 1, 1, 1);
    }
    ++row;
    glyout->addWidget(new QLabel(sl.at(row - 1), swjDialog), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
    {
        float value = static_cast<float>(OscFunc->SWJRecord.idleTime[i]);
        value = value / 100;
        glyout->addWidget(new QLabel(str.setNum(value, 'f', 2), swjDialog), row, i + 1, 1, 1);
    }
    ++row;
    glyout->addWidget(new QLabel(sl.at(row - 1), swjDialog), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
    {
        float value = static_cast<float>(OscFunc->SWJRecord.inaccuracy[i]);
        value = value / 100;
        glyout->addWidget(new QLabel(str.setNum(value, 'f', 2), swjDialog), row, i + 1, 1, 1);
    }
    ++row;
    glyout->addWidget(new QLabel(sl.at(row - 1), swjDialog), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
    {
        float value = OscFunc->SWJRecord.tInside[i];
        if (value == std::numeric_limits<float>::max())
            glyout->addWidget(new QLabel("-", swjDialog), row, i + 1, 1, 1);
        else
            glyout->addWidget(new QLabel(str.setNum(value, 'f', 2), swjDialog), row, i + 1, 1, 1);
    }
    ++row;
    glyout->addWidget(new QLabel(sl.at(row - 1), swjDialog), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
    {
        float value;
        value = OscFunc->SWJRecord.phyd[i];
        if (value == std::numeric_limits<float>::max())
            glyout->addWidget(new QLabel("-", swjDialog), row, i + 1, 1, 1);
        else
            glyout->addWidget(new QLabel(str.setNum(value, 'f', 2), swjDialog), row, i + 1, 1, 1);
    }
    vlyout->addLayout(glyout);
    // QPushButton *pb = new QPushButton("Сохранить журнал в файл");
    // connect(pb,SIGNAL(clicked(bool)),this,SLOT(SWJDialog::SaveSWJ()));
    // vlyout->addWidget(pb);
    // setLayout(vlyout);
    /*lyout = new QVBoxLayout;
    lyout->addWidget(TuneTW);
    setLayout(lyout);*/
    swjDialog->setLayout(vlyout);
    swjDialog->show();
}

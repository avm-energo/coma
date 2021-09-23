#include "oscmanager.h"

#include "../gen/files.h"
#include "../gen/modules.h"
#include "../gen/s2.h"
#include "../gen/std_ext.h"
#include "../gen/timefunc.h"
#include "../widgets/wd_func.h"
#include "eoscillogram.h"
#include "parseid10001.h"
#include "parseid10020.h"
#include "parseid10030.h"
#include "parseid10031.h"
#include "parseid9000.h"
#include "parseid9050.h"
#include "swjdialog.h"

OscManager::~OscManager()
{
}

void OscManager::loadOscFromFile(const QString &filename)
{
    QByteArray buffer;
    if (Files::LoadFromFile(filename, buffer) != Error::NoError)
        return;
}

void OscManager::loadOsc(TrendViewModel *model)
{
    trendDialog = UniquePointer<TrendViewDialog>(new TrendViewDialog);
    model->xmax = (static_cast<float>(model->Len / 2));
    model->xmin = -model->xmax;

    switch (model->idOsc)
    {
    case AVTUK_85::OSC_ID:
    {

        trendDialog->setAnalogNames(model->tmpav_85);
        trendDialog->setDigitalNames(model->tmpdv_85);
        trendDialog->setDigitalColors(model->dcolors_85);
        trendDialog->setAnalogColors(model->acolors_85);
        trendDialog->setDiscreteDescriptions(model->ddescr_85);
        trendDialog->setAnalogDescriptions(model->adescr_85);
        trendDialog->setRange(model->xmin, model->xmax, -200, 200);
        break;
    }
    case AVTUK_8X::OSC_ID:
    {
        model->tmpdv_80.clear();
        trendDialog->setAnalogNames(model->tmpav_80);
        trendDialog->setDigitalNames(model->tmpdv_80);
        trendDialog->setDigitalColors(model->dcolors_80);
        trendDialog->setAnalogColors(model->acolors_80);
        trendDialog->setRange(model->xmin, model->xmax, -200, 200);
        break;
    }

    case AVTUK_21::OSC_ID_MIN:
    {
        trendDialog->setAnalogColors(model->acolors_21);
        trendDialog->setAnalogNames(model->tmpav_21);
        trendDialog->setAnalogDescriptions(model->adescr_21);
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

        trendDialog->setAnalogNames(model->tmpav_85);
        trendDialog->setDigitalNames(model->tmpdv_85);
        trendDialog->setDigitalColors(model->dcolors_85);
        trendDialog->setAnalogColors(model->acolors_85);
        trendDialog->setRange(model->xmin, model->xmax, -200, 200);
        break;
    }
    }
    trendDialog->setTrendModel(model);
    trendDialog->setupPlots();
    trendDialog->setupUI();
    trendDialog->showPlot();
    trendDialog->show();
}

std::unique_ptr<TrendViewModel> OscManager::load(const FileStruct &fs)
{
    if (!oscHeader)
    {
        qCritical() << Error::DescError;
        return {};
    }
    return load(oscHeader.value(), fs);
}

std::unique_ptr<TrendViewModel> OscManager::load(const Record &record, const FileStruct &fs)
{
    quint16 curFileNum = std_ext::to_underlying(fs.filenum);

    constexpr size_t minSize = sizeof(S2DataTypes::OscHeader) + sizeof(S2DataTypes::DataRecHeader);
    if (fs.filedata.size() <= minSize)
    {
        qCritical() << Error::SizeError;
        return {};
    }

    auto filename = generateFilename(curFileNum, record.time);

    auto trendViewModel = std::make_unique<TrendViewModel>(record.len);

    {
        trendViewModel->SetFilename(filename);
        trendViewModel->SaveID(curFileNum);
        trendViewModel->Len = record.len;
        trendViewModel->xmax = float(record.len) / 2;
        trendViewModel->xmin = -trendViewModel->xmax;
    }

    std::unique_ptr<ParseModule> parseModule;
    switch (curFileNum)
    {
    case AVTUK_85::OSC_ID:
    {
        parseModule = std::make_unique<ParseID10030>(fs.filedata);
        break;
    }
    case AVTUK_8X::OSC_ID:
    {
        parseModule = std::make_unique<ParseID10020>(fs.filedata);
        break;
    }
    case AVTUK_21::OSC_ID_MIN:
    case 10002:
    case 10003:
    case 10004:
    case 10005:
    case 10006:
    case 10007:
    case 10008:
    case 10009:
    case 10010:
    case 10011:
    case 10012:
    case 10013:
    case 10014:
    case 10015:
    case AVTUK_21::OSC_ID_MAX:

    {
        parseModule = std::make_unique<ParseID10001>(fs.filedata);
        break;
    }

    default:
        return {};
    }
    bool result = parseModule->Parse(curFileNum, record, trendViewModel.get());
    if (!result)
    {
        qCritical() << Error::GeneralError;
        return {};
    }

    return trendViewModel;
}

void OscManager::loadSwjFromFile(const QString &filename)
{

    QByteArray buffer;

    if (Files::LoadFromFile(filename, buffer) != Error::NoError)
        return;

    DataTypes::S2FilePack outlist;
    S2::RestoreData(buffer, outlist);

    S2DataTypes::SwitchJourRecord journalRecord;

    bool foundSwj = false, foundOsc = false;
    for (auto &&file : outlist)
    {
        if (file.data.size() == sizeof(S2DataTypes::SwitchJourRecord))
        {
            journalRecord = *reinterpret_cast<S2DataTypes::SwitchJourRecord *>(file.data.data());
            foundSwj = true;
        }
        if ((file.ID == AVTUK_85::OSC_ID) || (file.ID == AVTUK_8X::OSC_ID))
        {
            foundOsc = true;
        }
    }

    if (!foundSwj)
    {
        qCritical() << Error::NoIdError;
        return;
    }

    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    auto OscFunc = std::make_unique<EOscillogram>();
    OscFunc->BA = buffer;
    SWJDialog *swjDialog = new SWJDialog(std::move(OscFunc), SWJDialog::SWJ_MODE_OFFLINE);
    swjDialog->GetSwjOscData();
    glyout->addWidget(new QLabel("Номер", swjDialog), 0, 0, 1, 1);
    glyout->addWidget(new QLabel("Дата, время", swjDialog), 0, 1, 1, 1);
    glyout->addWidget(new QLabel("Аппарат", swjDialog), 0, 2, 1, 2);
    glyout->addWidget(new QLabel("Переключение", swjDialog), 0, 4, 1, 2);
    glyout->addWidget(new QLabel(QString::number(journalRecord.num), swjDialog), 1, 0, 1, 1);
    glyout->addWidget(new QLabel(TimeFunc::UnixTime64ToString(journalRecord.time), swjDialog), 1, 1, 1, 1);
    QString str, tmps;
    QStringList tmpsl { "CB", "G", "D" };
    if (journalRecord.typeA == 1)
        tmps = tmpsl.at(0); //: "N/A";
    else if (journalRecord.typeA == 2)
        tmps = tmpsl.at(1);
    else if (journalRecord.typeA == 4)
        tmps = tmpsl.at(2);
    else
        tmps = "N/A";
    glyout->addWidget(new QLabel(tmps, swjDialog), 1, 2, 1, 1);
    glyout->addWidget(new QLabel(QString::number(journalRecord.numA), swjDialog), 1, 3, 1, 1);
    tmps = (journalRecord.options & 0x00000001) ? "ВКЛЮЧЕНИЕ" : "ОТКЛЮЧЕНИЕ";
    glyout->addWidget(new QLabel(tmps, swjDialog), 1, 4, 1, 2);
    glyout->addWidget(new QLabel("Тип коммутации:", swjDialog), 3, 0, 1, 4);

    quint32 tmpi32 = (journalRecord.options >> 1) & 0x03;
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
    tmps = (journalRecord.result) ? "НЕУСПЕШНО" : "УСПЕШНО";
    glyout->addWidget(new QLabel(tmps, swjDialog), 4, 4, 1, 1);

    glyout->addWidget(new QLabel("Коммутируемые фазы:", swjDialog), 5, 0, 1, 4);
    for (int i = 0; i < 4; i++)
    {
        if (((journalRecord.options >> 3) == i))
        {
            tmps = phases.at(i);
        }
    }
    glyout->addWidget(new QLabel(tmps, swjDialog), 5, 4, 1, 1);

    glyout->addWidget(new QLabel("Напряжение питания цепей соленоидов, В:", swjDialog), 6, 0, 1, 4);

    if (journalRecord.supplyVoltage == std::numeric_limits<float>::max())
        glyout->addWidget(new QLabel("-", swjDialog), 6, 4, 1, 1);
    else
        glyout->addWidget(new QLabel(QString::number(journalRecord.supplyVoltage), swjDialog), 6, 4, 1, 1);

    glyout->addWidget(new QLabel("Температура окружающей среды, Град:", swjDialog), 7, 0, 1, 4);

    if (journalRecord.tOutside == std::numeric_limits<float>::max())
        glyout->addWidget(new QLabel("-", swjDialog), 7, 4, 1, 1);
    else
        glyout->addWidget(new QLabel(QString::number(journalRecord.tOutside), swjDialog), 7, 4, 1, 1);

    if (foundOsc)
    {
        glyout->addWidget(new QLabel("Осциллограмма:", swjDialog), 8, 0, 1, 4);
        QPushButton *pb = new QPushButton("Открыть осциллограмму", swjDialog);
        pb->setIcon(QIcon(":/icons/osc.svg"));
        QObject::connect(pb, &QPushButton::clicked, swjDialog, [&] {
            swjDialog->trendViewDialog()->showPlot();
            swjDialog->trendViewDialog()->show();
        });
        glyout->addWidget(pb, 8, 4, 1, 1);
    }
    else
    {
        QPixmap *pm = new QPixmap(":/icons/hr.png");
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
        glyout->addWidget(new QLabel(QString::number(journalRecord.amperage[i], 'f', 1), swjDialog), row, i + 1, 1, 1);
    ++row;
    glyout->addWidget(new QLabel(sl.at(row - 1), swjDialog), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
        glyout->addWidget(new QLabel(QString::number(journalRecord.voltage[i], 'f', 1), swjDialog), row, i + 1, 1, 1);
    ++row;
    glyout->addWidget(new QLabel(sl.at(row - 1), swjDialog), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
    {
        float value = static_cast<float>(journalRecord.ownTime[i]);
        value = value / 100;
        glyout->addWidget(new QLabel(str.setNum(value, 'f', 2), swjDialog), row, i + 1, 1, 1);
    }
    ++row;
    glyout->addWidget(new QLabel(sl.at(row - 1), swjDialog), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
    {
        float value = static_cast<float>(journalRecord.fullTime[i]);
        value = value / 100;
        glyout->addWidget(new QLabel(str.setNum(value, 'f', 2), swjDialog), row, i + 1, 1, 1);
    }
    ++row;
    glyout->addWidget(new QLabel(sl.at(row - 1), swjDialog), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
    {
        float value = static_cast<float>(journalRecord.movTime[i]);
        value = value / 100;
        glyout->addWidget(new QLabel(str.setNum(value, 'f', 2), swjDialog), row, i + 1, 1, 1);
    }
    ++row;
    glyout->addWidget(new QLabel(sl.at(row - 1), swjDialog), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
    {
        float value = static_cast<float>(journalRecord.archTime[i]);
        value = value / 100;
        glyout->addWidget(new QLabel(str.setNum(value, 'f', 2), swjDialog), row, i + 1, 1, 1);
    }
    ++row;
    glyout->addWidget(new QLabel(sl.at(row - 1), swjDialog), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
    {
        float value = static_cast<float>(journalRecord.idleTime[i]);
        value = value / 100;
        glyout->addWidget(new QLabel(str.setNum(value, 'f', 2), swjDialog), row, i + 1, 1, 1);
    }
    ++row;
    glyout->addWidget(new QLabel(sl.at(row - 1), swjDialog), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
    {
        float value = static_cast<float>(journalRecord.inaccuracy[i]);
        value = value / 100;
        glyout->addWidget(new QLabel(str.setNum(value, 'f', 2), swjDialog), row, i + 1, 1, 1);
    }
    ++row;
    glyout->addWidget(new QLabel(sl.at(row - 1), swjDialog), row, 0, 1, 1);
    for (int i = 0; i < 3; ++i)
    {
        float value = journalRecord.tInside[i];
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
        value = journalRecord.phyd[i];
        if (value == std::numeric_limits<float>::max())
            glyout->addWidget(new QLabel("-", swjDialog), row, i + 1, 1, 1);
        else
            glyout->addWidget(new QLabel(str.setNum(value, 'f', 2), swjDialog), row, i + 1, 1, 1);
    }
    vlyout->addLayout(glyout);

    swjDialog->setLayout(vlyout);
    swjDialog->show();
}

File::Vector OscManager::loadFromFile(const QString &filename)
{
    QByteArray buffer;
    if (Files::LoadFromFile(filename, buffer) != Error::NoError)
        return {};

    DataTypes::S2FilePack outlist;
    S2::RestoreData(buffer, outlist);

    File::Vector vector;
    bool status = loadRecords(outlist, vector);
    if (!status)
    {
        qWarning() << Error::ReadError;
    }
    return vector;
    // if (status)
    //{
    //
    //  }
    //  loadOsc(model.get());
}

bool OscManager::loadRecords(const DataTypes::S2FilePack &input, File::Vector &output)
{
    if (input.size() < 2)
    {
        qWarning() << Error::SizeError << "Not enough records";
        return false;
    }

    auto foundOscHeader = std::find_if(input.cbegin(), input.cend(), isOscHeader);

    if (foundOscHeader == std::cend(input))
    {
        qWarning() << Error::DescError << "No osc header";
        return false;
    }

    auto header = loadCommon({ DataTypes::FilesEnum(foundOscHeader->ID), foundOscHeader->data });
    output.push_back(header);
    auto foundOsc = std::find_if(input.cbegin(), input.cend(), isOsc);

    if (foundOsc == std::cend(input))
    {
        qWarning() << Error::DescError << "No osc";
        return false;
    }
    auto model = load(header, { DataTypes::FilesEnum(foundOsc->ID), foundOsc->data });

    if (!model)
    {
        qWarning() << Error::ReadError;
        return false;
    }
    output.push_back(std::move(model));
    return true;
}

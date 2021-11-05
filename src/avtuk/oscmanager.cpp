#include "oscmanager.h"

#include "../gen/files.h"
#include "../gen/modules.h"
#include "../gen/s2.h"
#include "../gen/timefunc.h"
#include "../widgets/wd_func.h"
#include "parseid10001.h"
#include "parseid10020.h"
#include "parseid10030.h"

OscManager::~OscManager()
{
}

void OscManager::loadOsc(TrendViewModel *model)
{
    trendDialog = UniquePointer<TrendViewDialog>(new TrendViewDialog);
    model->setXmax((static_cast<float>(model->length() / 2)));
    model->setXmin(-model->xmax());

    trendDialog->setAnalogNames(model->analogValues());
    trendDialog->setDigitalNames(model->digitalValues());
    trendDialog->setDigitalColors(model->digitalColors());
    trendDialog->setAnalogColors(model->analogColors());
    trendDialog->setDiscreteDescriptions(model->digitalDescriptions());
    trendDialog->setAnalogDescriptions(model->analogDescriptions());
    switch (model->idOsc())
    {
    case AVTUK_85::OSC_ID:
    {
        trendDialog->setRange(model->xmin(), model->xmax(), -200, 200);
        break;
    }
    case AVTUK_8X::OSC_ID:
    {
        trendDialog->setRange(model->xmin(), model->xmax(), -200, 200);
        break;
    }

    case AVTUK_21::OSC_ID_MIN:
    {
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
        trendDialog->setRange(model->xmin(), model->xmax(), -200, 200);
        break;
    }
    }
    trendDialog->setTrendModel(model);
    trendDialog->setupPlots();
    trendDialog->setupUI();
    trendDialog->showPlot();
    trendDialog->show();
    qDebug() << trendDialog->size();
    trendDialog->adjustSize();
}

std::unique_ptr<TrendViewModel> OscManager::load(const FileStruct &fs) const
{
    if (!oscHeader)
    {
        qCritical() << Error::DescError;
        return {};
    }
    return load(oscHeader.value(), fs);
}

std::unique_ptr<TrendViewModel> OscManager::load(const Record &record, const FileStruct &fs) const
{
    auto curFileNum = fs.ID;

    constexpr size_t minSize = sizeof(S2DataTypes::OscHeader) + sizeof(S2DataTypes::DataRecHeader);
    if (fs.data.size() <= minSize)
    {
        qCritical() << Error::SizeError;
        return {};
    }
    std::unique_ptr<TrendViewModel> trendViewModel;
    std::unique_ptr<ParseModule> parseModule;
    switch (curFileNum)
    {
    case AVTUK_85::OSC_ID:
    {
        parseModule = std::make_unique<ParseID10030>(fs.data);
        trendViewModel = std::make_unique<TrendViewModel85>((record.len));
        break;
    }
    case AVTUK_8X::OSC_ID:
    {
        parseModule = std::make_unique<ParseID10020>(fs.data);
        trendViewModel = std::make_unique<TrendViewModel80>((record.len));
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
        parseModule = std::make_unique<ParseID10001>(fs.data);
        trendViewModel = std::make_unique<TrendViewModel21>((record.len));
        break;
    }

    default:
        return {};
    }

    {
        trendViewModel->setIdOsc(curFileNum);
        trendViewModel->setLength(record.len);
        trendViewModel->setXmax(float(record.len) / 2);
        trendViewModel->setXmin(-trendViewModel->xmax());
    }

    bool result = parseModule->Parse(curFileNum, record, trendViewModel.get());
    if (!result)
    {
        qCritical() << Error::GeneralError;
        return {};
    }

    return trendViewModel;
}

File::Vector OscManager::loadFromFile(const QString &filename) const
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
}

bool OscManager::loadRecords(const DataTypes::S2FilePack &input, File::Vector &output) const
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

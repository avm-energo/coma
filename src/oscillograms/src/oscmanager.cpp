#include "oscillograms/oscmanager.h"

#include <gen/files.h>
#include <gen/timefunc.h>
#include <oscillograms/parseid10001.h>
#include <oscillograms/parseid10020.h>
#include <oscillograms/parseid10022.h>
#include <oscillograms/parseid10023.h>
#include <oscillograms/parseid10030.h>
#include <oscillograms/parseid10040.h>
#include <oscillograms/parseid10050.h>
#include <s2/s2util.h>
#include <widgets/wd_func.h>

OscManager::~OscManager()
{
}

void OscManager::loadOsc(TrendViewModel *model)
{
    trendDialog = new TrendViewDialog;
    model->setXmax((static_cast<float>(model->length() / 2)));
    model->setXmin(-model->xmax());
    trendDialog->setModel(model);

    switch (model->idOsc())
    {
    case AVTUK_85::OSC_ID:
    case AVTUK_8X::OSC_ID:
    case AVTUK_KDV::OSC_ID:
    case AVTUK_KDV::SPEC_ID:
    case AVTUK_KIV::OSC_ALL_ID:
    case AVTUK_KIV::OSC_A_ID:
    case AVTUK_KIV::OSC_B_ID:
    case AVTUK_KIV::OSC_C_ID:
    case AVTUK_KIV::OSC_VIBR_ID:
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
    }

    trendDialog->setupPlots();
    trendDialog->setupUI();
    trendDialog->showPlot();
    trendDialog->show();
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
    constexpr auto minSize = sizeof(S2::OscHeader) + sizeof(S2::DataRecHeader);
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
    case AVTUK_KDV::OSC_ID:
    {
        parseModule = std::make_unique<ParseID10040>(fs.data);
        trendViewModel = std::make_unique<TrendViewModelKDV>(record.len);
        break;
    }
    case AVTUK_KDV::OSCV_ID:
    {
        parseModule = std::make_unique<ParseID10040>(fs.data);
        trendViewModel = std::make_unique<TrendViewModelKDVV>(record.len);
        break;
    }
    case AVTUK_KDV::SPEC_ID:
    {
        parseModule = std::make_unique<ParseID10050>(fs.data);
        trendViewModel = std::make_unique<TrendViewModelKDVSpec>(record.len);
        break;
    }
    case AVTUK_KDV::SPECV_ID:
    {
        parseModule = std::make_unique<ParseID10050>(fs.data);
        trendViewModel = std::make_unique<TrendViewModelKDVVSpec>(record.len);
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
    case AVTUK_KIV::OSC_ALL_ID:
    {
        parseModule = std::make_unique<ParseID10022>(fs.data);
        trendViewModel = std::make_unique<TrendViewModelKIV>(record.len);
        break;
    }
    case AVTUK_KIV::OSC_A_ID:
    case AVTUK_KIV::OSC_B_ID:
    case AVTUK_KIV::OSC_C_ID:
    case AVTUK_KIV::OSC_VIBR_ID:
    {
        parseModule = std::make_unique<ParseID10023>(fs.data);
        trendViewModel = std::make_unique<TrendViewModelKIVOne>(curFileNum, record.len);
        break;
    }
    default:
        return {};
    }
    trendViewModel->setIdOsc(curFileNum);
    trendViewModel->setLength(record.len);
    trendViewModel->setXmax(float(record.len) / 2);
    trendViewModel->setXmin(-trendViewModel->xmax());

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
    S2Util::RestoreData(buffer, outlist);

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

    auto header = loadCommon({ S2::FilesEnum(foundOscHeader->ID), foundOscHeader->data });
    output.push_back(header);
    auto foundOsc = std::find_if(input.cbegin(), input.cend(), isOsc);

    if (foundOsc == std::cend(input))
    {
        qWarning() << Error::DescError << "No osc";
        return false;
    }
    auto model = load(header, { S2::FilesEnum(foundOsc->ID), foundOsc->data });

    if (!model)
    {
        qWarning() << Error::ReadError;
        return false;
    }
    output.push_back(std::move(model));
    return true;
}

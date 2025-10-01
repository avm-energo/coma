#include "oscillograms/oscmanager.h"

#include <gen/files.h>
#include <gen/timefunc.h>
#include <oscillograms/parsers/parseid10001.h>
#include <oscillograms/parsers/parseid10020.h>
#include <oscillograms/parsers/parseid10022.h>
#include <oscillograms/parsers/parseid10023.h>
#include <oscillograms/parsers/parseid10030.h>
#include <oscillograms/parsers/parseid10040.h>
#include <oscillograms/parsers/parseid10050.h>
#include <s2/s2util.h>

OscManager::~OscManager() { }

TrendViewModel *OscManager::load(const QString &filename)
{
    File::Vector fileVector = loadFromFile(filename);
    TrendViewModel *oscModel = nullptr;
    for (auto &item : fileVector)
    {
        std::visit(
            overloaded {
                [&](S2::OscHeader &header) { setHeader(header); },                      //
                [](auto &&arg) { Q_UNUSED(arg) },                                       //
                [&](std::unique_ptr<TrendViewModel> &model) { oscModel = model.get(); } //
            },
            item);
    }
    setRangesById(oscModel);
    return oscModel;
}

void OscManager::setRangesById(TrendViewModel *model)
{
    if (!model)
        return;

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
        float xmax = (static_cast<float>(model->length() / 2));
        model->setRanges(-xmax, xmax, -200.0, +200.0);
        break;
    }
    case AVTUK_21::OSC_ID_MIN:
    {
        // 10000 мс, 20 мА (сделать автонастройку в зависимости от конфигурации по данному каналу)
        model->setRanges(0, 10000, -20.0, +20.0);
        break;
    }
    }
}

TrendViewModel *OscManager::load(const FileStruct &fs) const
{
    if (!oscHeader)
    {
        qDebug() << Error::DescError;
        return {};
    }
    return load(oscHeader.value(), fs);
}

TrendViewModel *OscManager::load(const Record &record, const FileStruct &fs) const
{
    auto curFileNum = fs.ID;
    constexpr auto minSize = sizeof(S2::OscHeader) + sizeof(S2::DataRecHeader);
    if (fs.data.size() <= minSize)
    {
        qDebug() << Error::SizeError;
        return {};
    }
    TrendViewModel *trendViewModel;
    ParseModule *parseModule;

    switch (curFileNum)
    {
    case AVTUK_85::OSC_ID:
    {
        parseModule = new ParseID10030(fs.data);
        trendViewModel = new TrendViewModel85(record.len);
        break;
    }
    case AVTUK_8X::OSC_ID:
    {
        parseModule = new ParseID10020(fs.data);
        trendViewModel = new TrendViewModel80(record.len);
        break;
    }
    case AVTUK_KDV::OSC_ID:
    {
        parseModule = new ParseID10040(fs.data);
        trendViewModel = new TrendViewModelKDV(record.len);
        break;
    }
    case AVTUK_KDV::OSCV_ID:
    {
        parseModule = new ParseID10040(fs.data);
        trendViewModel = new TrendViewModelKDVV(record.len);
        break;
    }
    case AVTUK_KDV::SPEC_ID:
    {
        parseModule = new ParseID10050(fs.data);
        trendViewModel = new TrendViewModelKDVSpec(record.len);
        break;
    }
    case AVTUK_KDV::SPECV_ID:
    {
        parseModule = new ParseID10050(fs.data);
        trendViewModel = new TrendViewModelKDVVSpec(record.len);
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
        parseModule = new ParseID10001(fs.data);
        trendViewModel = new TrendViewModel21(record.len);
        break;
    }
    case AVTUK_KIV::OSC_ALL_ID:
    {
        parseModule = new ParseID10022(fs.data);
        trendViewModel = new TrendViewModelKIV(record.len);
        break;
    }
    case AVTUK_KIV::OSC_A_ID:
    case AVTUK_KIV::OSC_B_ID:
    case AVTUK_KIV::OSC_C_ID:
    case AVTUK_KIV::OSC_VIBR_ID:
    {
        parseModule = new ParseID10023(fs.data);
        trendViewModel = new TrendViewModelKIVOne(curFileNum, record.len);
        break;
    }
    default:
        return {};
    }
    trendViewModel->setIdOsc(curFileNum);
    trendViewModel->setLength(record.len);

    bool result = parseModule->Parse(curFileNum, record, trendViewModel);
    if (!result)
    {
        qDebug() << Error::GeneralError;
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
    output.push_back(model);
    return true;
}

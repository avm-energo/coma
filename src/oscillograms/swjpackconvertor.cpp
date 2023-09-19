#include "swjpackconvertor.h"

#include "../widgets/epopup.h"
#include "../widgets/wd_func.h"
#include "swjmanager.h"

#include <QDir>
#include <gen/timefunc.h>
#include <xlsxdocument.h>

SwjPackConvertor::SwjPackConvertor(QWidget *parent) : QObject(parent), m_parent(parent)
{
}

void SwjPackConvertor::readFile(const QString &swjfilepath)
{
    SwjManager swjManager;
    SwjModel swjModel;
    auto swjVector = swjManager.loadFromFile(swjfilepath);
    for (auto &item : swjVector)
    {
        std::visit(overloaded //
            {
                [this](SwjModel &model) { m_data.push_back(std::move(model)); }, //
                [](S2::OscHeader &header) { Q_UNUSED(header); },                 //
                [](std::unique_ptr<TrendViewModel> &model) { Q_UNUSED(model); }  //
            },
            item);
    }
}

void SwjPackConvertor::sortData()
{
    std::sort(m_data.begin(), m_data.end(), [](SwjModel &lhs, SwjModel &rhs) -> bool {
        auto commonLhs = lhs.commonModel.get();
        auto commonRhs = rhs.commonModel.get();
        auto dateLhsStr = commonLhs->data(commonLhs->index(1, 1)).toString();
        auto dateRhsStr = commonRhs->data(commonRhs->index(1, 1)).toString();
        auto lhsDateTime = QDateTime::fromString(dateLhsStr, "yyyy/MM/dd hh:mm:ss.zzz");
        auto rhsDateTime = QDateTime::fromString(dateRhsStr, "yyyy/MM/dd hh:mm:ss.zzz");
        return lhsDateTime < rhsDateTime;
    });
}

void SwjPackConvertor::writeHeader(QXlsx::Worksheet *sheet)
{
    const static QStringList header {
        "Дата и время",                                                 //
        "Переключение",                                                 //
        "Действующее значение тока в момент коммутации ф. A, А",        //
        "Действующее значение тока в момент коммутации ф. B, А",        //
        "Действующее значение тока в момент коммутации ф. C, А",        //
        "Действующее значение напряжения в момент коммутации ф. A, кВ", //
        "Действующее значение напряжения в момент коммутации ф. B, кВ", //
        "Действующее значение напряжения в момент коммутации ф. C, кВ", //
        "Собственное время коммутации ф. A, мс",                        //
        "Собственное время коммутации ф. B, мс",                        //
        "Собственное время коммутации ф. C, мс",                        //
        "Полное время коммутации ф. A, мс",                             //
        "Полное время коммутации ф. B, мс",                             //
        "Полное время коммутации ф. C, мс",                             //
        "Время перемещения главного контакта ф. A, мс",                 //
        "Время перемещения главного контакта ф. B, мс",                 //
        "Время перемещения главного контакта ф. C, мс",                 //
        "Время горения дуги ф. A, мс",                                  //
        "Время горения дуги ф. B, мс",                                  //
        "Время горения дуги ф. C, мс",                                  //
        "Время безоперационного простоя к моменту коммутации ф. A, ч",  //
        "Время безоперационного простоя к моменту коммутации ф. B, ч",  //
        "Время безоперационного простоя к моменту коммутации ф. C, ч",  //
        "Погрешность синхронной коммутации ф. A, мс",                   //
        "Погрешность синхронной коммутации ф. B, мс",                   //
        "Погрешность синхронной коммутации ф. C, мс",                   //
        "Температура внутри привода ф. A, Град",                        //
        "Температура внутри привода ф. B, Град",                        //
        "Температура внутри привода ф. C, Град",                        //
        "Давление в гидросистеме привода ф. A, Па",                     //
        "Давление в гидросистеме привода ф. B, Па",                     //
        "Давление в гидросистеме привода ф. C, Па"                      //
    };

    int column = 1;
    int row = 1;
    for (const auto &label : header)
    {
        sheet->writeString(QXlsx::CellReference { row, column }, label);
        ++column;
    }
}

void SwjPackConvertor::writeData(QXlsx::Worksheet *sheet)
{
    int row = 2;
    for (auto &item : m_data)
    {
        int column = 1;
        auto common = item.commonModel.get();
        auto detail = item.detailModel.get();

        auto dateStrList = common->data(common->index(1, 1)).toString().split(" ");
        auto switchType = common->data(common->index(3, 1)).toString();
        dateStrList[0].replace("/", "-");
        dateStrList[1].replace(".", ",");
        auto dateStr = dateStrList.join(' ');
        sheet->writeString(QXlsx::CellReference { row, column++ }, dateStr);
        sheet->writeString(QXlsx::CellReference { row, column++ }, switchType);

        for (int indexRow = 1; indexRow < 32; indexRow++)
        {
            for (int indexCol = 1; indexCol < 4; indexCol++)
            {
                auto detailData = detail->data(detail->index(indexRow, indexCol)).toString();
                detailData.replace(".", ",");
                sheet->writeString(QXlsx::CellReference { row, column++ }, detailData);
            }
        }

        row++;
    }
}

void SwjPackConvertor::writeToFile(const QString &filepath)
{
    if (QFile::exists(filepath))
        QFile::remove(filepath);

    auto doc = new QXlsx::Document(filepath, this);
    auto workSheet = doc->currentWorksheet();
    writeHeader(workSheet);
    writeData(workSheet);
    doc->save();
    doc->deleteLater();
    EMessageBox::information(m_parent, "Файл записан успешно");
}

void SwjPackConvertor::selectDirectory()
{
    auto dirPath = WDFunc::ChooseDirectoryForOpen(m_parent);
    if (!dirPath.isEmpty())
    {
        QDir swjDirectory(dirPath);
        auto entryFiles = swjDirectory.entryList(QDir::Filter::Files);
        entryFiles = entryFiles.filter(".swj");
        if (!entryFiles.isEmpty())
        {
            for (const auto &file : qAsConst(entryFiles))
                readFile(dirPath + "/" + file);
            sortData();
            auto excelFilepath = WDFunc::ChooseFileForSave(m_parent, "Excel files (*.xlsx)", "xlsx", "test");
            if (!excelFilepath.isEmpty())
                writeToFile(excelFilepath);
            else
                EMessageBox::warning(m_parent, "Не выбран выходной файл");
        }
        else
            EMessageBox::warning(m_parent, "В папке отсутствуют файлы формата swj");
    }
}

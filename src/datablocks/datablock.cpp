/* DataBlock - a block description with its own view and behaviour:
 *
 * Blocks are structures of types: Bac (tune block), Bda (data without correction) and Bd (current data)
 *
 * To create block for the type Bac struct S have a constructor
 * DataBlock *block = new DataBlock(&S, "Struct S", sizeof(S), DataBlock::DataBlockTypes::BacBlock, 0);
 *
 * Then add each element of the struct to datamodel:
 * block->addNewItem("S.s1", &S.s1, "S first item");
 * block->addNewItem("secitem", &S.s2, "S second item");
 *
 * after all the elements are set invoke method:
 * block->setWidget();
 *
 * then you can add widget to your GUI:
 * someLayout->addWidget(block);
 *
 * widget DataBlock has the following slots (methods):
 * updateFromWidget() - to get values from widget and write them to the block(i.e. our struct S will have actual values)
 * updateWidget() - to set values from block to the widget
 * writeBlockToModule() - write block to the module through usb according to its type
 * readBlockFromModule() - reverse process to get data from module and write it to the block
 *
 * */

#include "datablock.h"

#include "../widgets/wd_func.h"

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QScrollBar>
#include <gen/files.h>
#include <gen/stdfunc.h>
#include <interfaces/conn/active_connection.h>

using namespace Interface;

DataBlock::DataBlock(QObject *parent) : QObject(parent)
{
    m_widgetIsSet = false;
    m_isBottomButtonsWidgetCreated = false;
    valueNumberCounter = 0;
}

DataBlock::~DataBlock()
{
}

void DataBlock::setBlock(const DataBlock::BlockStruct &bds)
{
    m_block = bds;
    setupValuesDesc();
}

QWidget *DataBlock::widget(bool showButtons)
{
    if (!m_widgetIsSet)
        createWidget();
    if (m_isBottomButtonsWidgetCreated)
        m_bottomButtonsWidget->setVisible(showButtons);
    setEnabled(showButtons);
    return m_widget;
}

void DataBlock::createWidget()
{
    auto mainWidget = new QWidget;
    auto scrollWidget = new QWidget;
    auto mainLayout = new QVBoxLayout;
    int count = 0;
    for (auto &group : m_valuesDesc)
    {
        if (group.values.size() == 1) // for only one parameter it doesn't need to make a groupbox
        {
            auto singleWidget = new QWidget(scrollWidget);
            auto widgetLayout = addBlockValueToWidget(group.values.first(), singleWidget);
            singleWidget->setLayout(widgetLayout);
            mainLayout->addWidget(singleWidget);
            continue;
        }
        auto gb = new QGroupBox(group.groupDesc, scrollWidget);
        auto gridLayout = new QGridLayout;
        auto itemsPerLine = StdFunc::goldenRatio(group.values.count());
        for (auto &valueDesc : group.values)
        {
            auto widgetLayout = addBlockValueToWidget(valueDesc, gb);
            gridLayout->addLayout(widgetLayout, count / itemsPerLine, count % itemsPerLine);
            ++count;
        }
        gb->setLayout(gridLayout);
        mainLayout->addWidget(gb);
    }
    scrollWidget->setLayout(mainLayout);
    auto scrollArea = new QScrollArea;
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(scrollWidget);
    mainLayout = new QVBoxLayout;
    mainLayout->addWidget(scrollArea);
    if (m_block.bottomButtonsVisible)
        mainLayout->addWidget(blockButtonsUI());
    mainWidget->setLayout(mainLayout);
    m_widget = mainWidget;
    m_widgetIsSet = true;
}

QHBoxLayout *DataBlock::addBlockValueToWidget(ValueStr &value, QWidget *parent)
{
    auto layout = new QHBoxLayout;
    auto textLabel = new QLabel(value.desc, parent);
    textLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(textLabel);

    auto valueLabel = new QLineEdit(parent);
    valueLabel->setToolTip(value.tooltip);
    valueLabel->setObjectName(value.valueId);
    valueLabel->setStyleSheet(ValuesFormat);
    layout->addWidget(valueLabel);
    return layout;
}

QWidget *DataBlock::blockButtonsUI()
{
    if (!m_isBottomButtonsWidgetCreated)
    {
        m_bottomButtonsWidget = new QWidget;
        QVBoxLayout *lyout = new QVBoxLayout;
        QDialogButtonBox *group = new QDialogButtonBox;

        const QList<QPair<QPair<QString, QString>, std::function<void()>>> funcs {
            { { "Получить", ":/icons/tnread.svg" }, [this]() { readAndUpdate(); } },
            { { "Записать", ":/icons/tnwrite.svg" }, [this]() { writeBlockToModule(); } },
            { { "Задать по умолчанию", ":/icons/tnyes.svg" }, [this]() { setDefBlockAndUpdate(); } },
            { { "Прочитать из файла и записать в устройство", ":/icons/tnload.svg" },
                [this]() { readFromFileUserChoose(); } },
            { { "Прочитать из устройства и сохранить в файл", ":/icons/tnsave.svg" },
                [this]() { saveToFileUserChoose(); } }
        };

        for (auto &i : funcs)
        {
            const QString &toolTip = i.first.first;
            group->addButton(WDFunc::NewHexagonPB(m_bottomButtonsWidget, "", i.second, i.first.second, toolTip),
                QDialogButtonBox::ActionRole);
        }
        group->setCenterButtons(true);
        lyout->addWidget(group);
        m_bottomButtonsWidget->setLayout(lyout);

        m_isBottomButtonsWidgetCreated = true;
    }
    return m_bottomButtonsWidget;
}

void DataBlock::setEnabled(bool isEnabled)
{
    for (auto &group : m_valuesDesc)
    {
        for (auto &valueDesc : group.values)
        {
            WDFunc::SetEnabled(m_widget, valueDesc.valueId, isEnabled);
        }
    }
}

DataBlock::BlockStruct DataBlock::block()
{
    return m_block;
}

void DataBlock::updateWidget()
{
    specificUpdateWidget();
    for (auto &group : m_valuesDesc)
    {
        for (auto &valueDesc : group.values)
        {
            std::visit(overloaded { [&](float *arg)                                                       //
                           {                                                                              //
                               WDFunc::SetLEData(                                                         //
                                   m_widget, valueDesc.valueId, WDFunc::StringFloatValueWithCheck(*arg)); //
                           },                                                                             //
                           [&](quint32 *arg)                                                              //
                           {                                                                              //
                               WDFunc::SetLEData(m_widget, valueDesc.valueId, QString::number(*arg));     //
                           } },                                                                           //
                valueDesc.value);
        }
    }
}

void DataBlock::updateFromWidget()
{
    specificUpdateFromWidget();
    for (auto &group : m_valuesDesc)
    {
        for (auto &valueDesc : group.values)
        {
            std::visit(
                [&](auto *arg) {
                    using T = std::remove_pointer_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, float>)
                    {
                        float tmpf = StdFunc::ToFloat(WDFunc::LEData(m_widget, valueDesc.valueId));
                        *arg = tmpf;
                    }
                    else if constexpr (std::is_same_v<T, quint32>)
                    {
                        quint32 tmpi = WDFunc::LEData(m_widget, valueDesc.valueId).toUInt();
                        *arg = tmpi;
                    }
                    //                        else
                    //                            static_assert(false, "non-exhaustive visitor!");
                },
                valueDesc.value);
        }
    }
}

Error::Msg DataBlock::writeBlockToModule()
{
    auto conn = ActiveConnection::sync();
    switch (m_block.blocktype)
    {
    case DataTypes::DataBlockTypes::BacBlock:
    {
        updateFromWidget();
        if (conn->writeBlockSync(
                m_block.blocknum, DataTypes::DataBlockTypes::BacBlock, m_block.block, m_block.blocksize)
            != Error::Msg::NoError)
        {
            qCritical("Не удалось записать блок");
            return Error::Msg::GeneralError;
        }
        break;
    }
    case DataTypes::DataBlockTypes::BdBlock:
    {
        WARNMSG("Writing data block BD is prohibited");
        break;
    }
    case DataTypes::DataBlockTypes::BdaBlock:
    {
        WARNMSG("Writing data block BDA is prohibited");
        break;
    }
    default:
        break;
    }
    return Error::Msg::NoError;
}

void DataBlock::readBlockFromModule()
{
    auto conn = ActiveConnection::sync();
    switch (m_block.blocktype)
    {
    case DataTypes::DataBlockTypes::BacBlock:
    case DataTypes::DataBlockTypes::BdBlock:
    case DataTypes::DataBlockTypes::BdaBlock:
    {
        const auto err = conn->reqBlockSync(m_block.blocknum, m_block.blocktype, m_block.block, m_block.blocksize);
        if (err != Error::Msg::NoError)
            qCritical("Не удалось прочитать блок");
        break;
    }
    case DataTypes::DataBlockTypes::BciBlock:
    {
        const auto err = conn->readS2FileSync(S2::FilesEnum::Config);
        if (err != Error::Msg::NoError)
            qCritical("Не удалось прочитать блок");
    }
    default:
        break;
    }
}

void DataBlock::readFromFileUserChoose()
{
    auto filepath = WDFunc::ChooseFileForOpen(nullptr, ExtMap[m_block.blocktype].mask);
    if (!filepath.isEmpty())
        loadFromFileAndWriteToModule(filepath);
}

Error::Msg DataBlock::loadFromFileAndWriteToModule(const QString &filename)
{
    QByteArray ba;
    if (Files::LoadFromFile(filename, ba) == Error::Msg::NoError)
    {
        memcpy(m_block.block, &ba.data()[0], m_block.blocksize);
        updateWidget();
        if (writeBlockToModule() == Error::Msg::NoError)
            return Error::Msg::NoError;
    }
    qCritical("Не удалось записать блок");
    return Error::Msg::GeneralError;
}

Error::Msg DataBlock::readFromFile()
{
    return loadFromFileAndWriteToModule(StdFunc::GetSystemHomeDir() + cpuIDFilenameStr());
}

Error::Msg DataBlock::saveToFile()
{
    QByteArray ba(static_cast<char *>(m_block.block), m_block.blocksize);
    QString filestr = StdFunc::GetSystemHomeDir() + cpuIDFilenameStr();
    return Files::SaveToFile(filestr, ba);
}

void DataBlock::saveToFileUserChoose()
{
    readBlockFromModule();
    auto filepath = WDFunc::ChooseFileForSave(nullptr, ExtMap[m_block.blocktype].mask, //
        ExtMap[m_block.blocktype].extension, cpuIDFilenameStr());
    if (filepath.isEmpty())
        return;
    QByteArray ba(static_cast<char *>(m_block.block), m_block.blocksize);
    Files::SaveToFile(filepath, ba);
}

void DataBlock::readAndUpdate()
{
    readBlockFromModule();
    updateWidget();
}

void DataBlock::setDefBlockAndUpdate()
{
    setDefBlock();
    if (m_widgetIsSet)
        updateWidget();
}

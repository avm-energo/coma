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

#include "../gen/files.h"
#include "../gen/stdfunc.h"
#include "../interfaces/baseinterface.h"
#include "../widgets/wd_func.h"

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QVBoxLayout>

DataBlock::DataBlock(QObject *parent) : QObject(parent)
{
    m_widgetIsSet = false;
    m_isBottomButtonsWidgetCreated = false;
}

DataBlock::~DataBlock()
{
}

void DataBlock::setBlock(const DataBlock::BlockStruct &bds)
{
    m_block = bds;
}

QWidget *DataBlock::widget(bool showButtons)
{
    if (!m_widgetIsSet)
        createWidget();
    if (m_isBottomButtonsWidgetCreated)
        m_bottomButtonsWidget->setVisible(showButtons);
    return m_widget;
}

DataBlock::BlockStruct DataBlock::block()
{
    return m_block;
}

void DataBlock::updateFromWidget()
{
}

Error::Msg DataBlock::writeBlockToModule()
{
    switch (m_block.blocktype)
    {
    case DataTypes::DataBlockTypes::BacBlock:
    {
        updateFromWidget();
        if (BaseInterface::iface()->writeBlockSync(
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
    switch (m_block.blocktype)
    {
    case DataTypes::DataBlockTypes::BacBlock:
    case DataTypes::DataBlockTypes::BdBlock:
    case DataTypes::DataBlockTypes::BdaBlock:
    {
        if (BaseInterface::iface()->reqBlockSync(m_block.blocknum, m_block.blocktype, m_block.block, m_block.blocksize)
            != Error::Msg::NoError)

            qCritical("Не удалось прочитать блок");
        break;
    }
    case DataTypes::DataBlockTypes::BciBlock:
    {
        if (BaseInterface::iface()->readS2FileSync(DataTypes::Config) != Error::Msg::NoError)
            qCritical("Не удалось прочитать блок");
    }
    default:
        break;
    }
}

void DataBlock::readFromFileUserChoose()
{
    loadFromFileAndWriteToModule(WDFunc::ChooseFileForOpen(nullptr, ExtMap[m_block.blocktype].mask));
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
    readBlockFromModule();
    QByteArray ba(static_cast<char *>(m_block.block), m_block.blocksize);
    QString filestr = StdFunc::GetSystemHomeDir() + cpuIDFilenameStr();
    return Files::SaveToFile(filestr, ba);
}

void DataBlock::saveToFileUserChoose()
{
    readBlockFromModule();
    QByteArray ba(static_cast<char *>(m_block.block), m_block.blocksize);
    Files::SaveToFile(WDFunc::ChooseFileForSave(nullptr, ExtMap[m_block.blocktype].mask,
                          ExtMap[m_block.blocktype].extension, cpuIDFilenameStr()),
        ba);
}

void DataBlock::readAndUpdate()
{
    readBlockFromModule();
    updateWidget();
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

void DataBlock::setDefBlockAndUpdate()
{
    setDefBlock();
    if (m_widgetIsSet)
        updateWidget();
}

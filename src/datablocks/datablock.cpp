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
 * updateModel() - to get values from widget and write them to the model (i.e. our struct S will have actual values)
 * updateWidget() - to set values from struct S to the widget
 * writeBlockToModule() - write struct S to the module through usb according to its type
 * readBlockFromModule() - reverse process to get data from module and write it to the struct S
 *
 * */

#include "datablock.h"

//#include "../models/valuedelegate.h"
#include "../gen/files.h"
#include "../gen/stdfunc.h"
#include "../interfaces/baseinterface.h"
#include "../widgets/wd_func.h"

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QVBoxLayout>

DataBlock::DataBlock(QObject *parent) : QObject(parent)
{
    //    m_blockNum = blocknum;
    //    m_blockType = blocktype;
    //    m_block = block;
    //    m_blockSize = blocksize;
    //    m_blockName = blockname;
    //    m_VModel = new ValueModel;
    //    m_curModelColumn = m_curModelRow = 0;
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

// void DataBlock::setModel(const QList<ValueItem *> &dd, int columnsnumber)
//{
//    m_VModel->setModel(dd, columnsnumber);
//}

// QWidget *DataBlock::widget()
//{
//    QWidget *w = new QWidget;
//    QVBoxLayout *lyout = new QVBoxLayout;
//    QVBoxLayout *vlyout = new QVBoxLayout;
//    QGroupBox *gb = new QGroupBox(m_block.caption);
//    ETableView *tv = new ETableView;
//    ValueDelegate *chdg = new ValueDelegate;
//    tv->setItemDelegate(chdg);
//    tv->setModel(m_VModel);
//    vlyout->addWidget(tv);
//    gb->setLayout(vlyout);
//    lyout->addWidget(gb);
//    w->setLayout(lyout);
//    return w;
//}

DataBlock::BlockStruct DataBlock::block()
{
    return m_block;
}

void DataBlock::updateFromWidget()
{
}

// void DataBlock::updateModel()
//{
//    m_VModel->updateModel();
//}

// void DataBlock::updateValues()
//{
//    m_VModel->updateFromModel();
//}

Error::Msg DataBlock::writeBlockToModule()
{
    switch (m_block.blocktype)
    {
    case DataTypes::DataBlockTypes::BacBlock:
    {
        //        if (update)
        //            updateValues();
        updateFromWidget();
        if (BaseInterface::iface()->writeBlockSync(
                m_block.blocknum, DataTypes::DataBlockTypes::BacBlock, m_block.block, m_block.blocksize)
            != Error::Msg::NoError)
        {
            //        if (Commands::WriteBac(m_block.blocknum, &m_block.block, m_block.blocksize) !=
            //        Error::Msg::NoError)
            //            return Error::Msg::GeneralError;
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
        //        if (Commands::GetBac(m_block.blocknum, &m_block.block, m_block.blocksize) != Error::Msg::NoError)
        if (BaseInterface::iface()->reqBlockSync(m_block.blocknum, m_block.blocktype, m_block.block, m_block.blocksize)
            != Error::Msg::NoError)

            qCritical("Не удалось прочитать блок");
        //            return Error::Msg::GeneralError;
        //        if (update)
        //            updateModel();
        break;
    }
        //    {
        //        if (Commands::GetBd(m_block.blocknum, &m_block.block, m_block.blocksize) != Error::Msg::NoError)
        //            return Error::Msg::GeneralError;
        //        //        if (update)
        //        //            updateModel();
        //        break;
        //    }
        //    {
        //        if (Commands::GetBda(m_block.blocknum, &m_block.block, m_block.blocksize) != Error::Msg::NoError)
        //            return Error::Msg::GeneralError;
        //        //        if (update)
        //        //            updateModel();
        //        break;
        //    }
    case DataTypes::DataBlockTypes::BciBlock:
    {
        if (BaseInterface::iface()->readS2FileSync(DataTypes::Config) != Error::Msg::NoError)
            qCritical("Не удалось прочитать блок");
        //        S2ConfigType *s2 = static_cast<S2ConfigType *>(m_block.block);
        //        return Commands::GetFileWithRestore(1, s2);
    }
    default:
        break;
    }
    //    emit m_VModel->dataChanged();

    //    return Error::Msg::NoError;
}

void DataBlock::readFromFileUserChoose()
{
    loadFromFileAndWriteToModule(WDFunc::ChooseFileForOpen(nullptr, ExtMap[m_block.blocktype].mask));
}

Error::Msg DataBlock::loadFromFileAndWriteToModule(const QString &filename)
{
    QByteArray ba;
    //    QString filestr = StdFunc::GetSystemHomeDir() + Board::GetInstance().UID() + ExtMap[m_block.blocktype]
    //        + QString::number(m_block.blocknum);
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
    //    QString filestr = StdFunc::GetSystemHomeDir() + Board::GetInstance().UID() + ExtMap[m_block.blocktype]
    //        + QString::number(m_block.blocknum);
    QString filestr = StdFunc::GetSystemHomeDir() + cpuIDFilenameStr();
    return Files::SaveToFile(filestr, ba);
}

void DataBlock::saveToFileUserChoose()
{
    readBlockFromModule();
    QByteArray ba(static_cast<char *>(m_block.block), m_block.blocksize);
    //    DataBlock::FilePropertiesStruct fst;
    //    DataBlock::getFileProperties(m_block.blocktype, fst);
    //    QString filestr = StdFunc::GetSystemHomeDir() + Board::GetInstance().UID() + ExtMap[m_block.blocktype]
    //        + QString::number(m_block.blocknum);
    Files::SaveToFile(WDFunc::ChooseFileForSave(nullptr, ExtMap[m_block.blocktype].mask,
                          ExtMap[m_block.blocktype].extension, cpuIDFilenameStr()),
        ba);
}

// void DataBlock::getFileProperties(DataTypes::DataBlockTypes type, FilePropertiesStruct &st)
//{
//    QString cpuid = Board::GetInstance().UID();
//    //    QString fileExt, fileName, fileMask;
//    if (type == DataTypes::DataBlockTypes::BacBlock)
//    {
//        st.extension = "tn";
//        st.mask = "Tune files (*.tn?)";
//    }
//    else if (type == DataTypes::DataBlockTypes::BciBlock)
//    {
//        st.extension = "cf";
//        st.mask = "Configuration files (*.cf?)";
//    }
//    st.filename = cpuid + "." + st.extension;
//}

void DataBlock::readAndUpdate()
{
    readBlockFromModule();
    updateWidget();
}

QWidget *DataBlock::blockButtonsUI()
{
    if (!m_isBottomButtonsWidgetCreated)
    {
        createBottomButtonsWidget();
        m_isBottomButtonsWidgetCreated = true;
    }
    return m_bottomButtonsWidget;
}

void DataBlock::createBottomButtonsWidget()
{
    m_bottomButtonsWidget = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QDialogButtonBox *group = new QDialogButtonBox;
    //    QHBoxLayout *hlyout = new QHBoxLayout;

    const QList<QPair<QPair<QString, QString>, std::function<void()>>> funcs {
        { { "Получить", ":/icons/tnread.svg" }, [this]() { readAndUpdate(); } },
        { { "Записать", ":/icons/tnwrite.svg" }, [this]() { writeBlockToModule(); } },
        { { "Задать по умолчанию", ":/icons/tnyes.svg" }, [this]() { setDefBlockAndUpdate(); } },
        { { "Прочитать из файла и записать в устройство", ":/icons/tnload.svg" },
            [this]() { readFromFileUserChoose(); } },
        { { "Прочитать из устройства и сохранить в файл", ":/icons/tnsave.svg" }, [this]() { saveToFileUserChoose(); } }
    };

    for (auto &i : funcs)
    {
        //        const QIcon &icon = i.first.second;
        const QString &toolTip = i.first.first;
        group->addButton(WDFunc::NewHexagonPB(m_bottomButtonsWidget, "", i.second, i.first.second, toolTip),
            QDialogButtonBox::ActionRole);

        //        QPushButton *pb = new QPushButton();
        //        pb->setObjectName("Hexagon");
        //        pb->setIcon(icon);
        //        pb->setAttribute(Qt::WA_Hover);
        //        pb->setAttribute(Qt::WA_X11NetWmWindowTypeToolBar);
        //        //        pb->setStyleSheet("background-color: rgba(255, 255, 255, 0);"
        //        //                          "    border-style: groove;"
        //        //                          "   border-color: #ADADAD;"
        //        //                          "border-width: 1px;");
        //        pb->setToolTip(toolTip);
        //        pb->setMinimumSize(50, 50);
        //        pb->setIconSize(QSize(50, 50));

        // pb->setIconSize(QSize(pb->minimumHeight() / 2, pb->minimumHeight() / 2));

        //        connect(pb, &QAbstractButton::clicked, this, i.second);
        //        if (StdFunc::IsInEmulateMode())
        //            pb->setEnabled(false);
        //        group->addButton(pb, QDialogButtonBox::ActionRole);
        // hlyout->addWidget(pb);
    }
    group->setCenterButtons(true);
    lyout->addWidget(group);
    // lyout->addLayout(hlyout);
    m_bottomButtonsWidget->setLayout(lyout);
}

void DataBlock::setDefBlockAndUpdate()
{
    setDefBlock();
    if (m_widgetIsSet)
        updateWidget();
}

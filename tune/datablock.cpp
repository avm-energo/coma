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
#include "../models/checkdelegate.h"
#include "../usb/commands.h"
#include "../widgets/wd_func.h"
#include <QGroupBox>
#include <QVBoxLayout>

DataBlock::DataBlock(void *block, const QString &blockname, int blocksize, DataBlock::DataBlockTypes blocktype,
    int blocknum, QWidget *parent)
    : QWidget(parent)
{
    m_blockNum = blocknum;
    m_blockType = blocktype;
    m_block = block;
    m_blockSize = blocksize;
    m_blockName = blockname;
    m_VModel = new ValueModel;
    m_curModelColumn = m_curModelRow = 0;
}

void DataBlock::addNewItem(const QString &name, void *ptr, const QString &tooltip)
{
    DataDescription dd;
    dd.name = name;
    dd.dataptr = ptr;
    dd.tooltip = tooltip;
    m_dataList.append(dd);
    m_VModel->setData(m_VModel->index(m_curModelRow, m_curModelColumn++), name);
    m_VModel->setData(
        m_VModel->index(m_curModelRow, m_curModelColumn), CheckDelegate::CheckStyles::OUTVALUE, Qt::UserRole);
    m_VModel->setData(m_VModel->index(m_curModelRow, m_curModelColumn), tooltip, Qt::ToolTipRole);
    m_VModel->setValueData(m_VModel->index(m_curModelRow, m_curModelColumn++), ptr);
    if (m_curModelColumn > MAX_VALUEMODEL_COLUMNS)
    {
        m_curModelRow++;
        m_curModelColumn = 0;
    }
}

void DataBlock::setWidget()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox(m_blockName);
    ETableView *tv = new ETableView;
    CheckDelegate *chdg = new CheckDelegate;
    tv->setItemDelegate(chdg);
    tv->setModel(m_VModel);
    vlyout->addWidget(tv);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);
    setLayout(lyout);
}

void DataBlock::updateModel() { m_VModel->updateModel(); }

void DataBlock::updateWidget() { m_VModel->updateFromModel(); }

Error::Msg DataBlock::writeBlockToModule(bool update)
{
    switch (m_blockType)
    {
    case DataBlockTypes::BacBlock:
    {
        if (update)
            updateModel();
        if (Commands::WriteBac(m_blockNum, &m_block, m_blockSize) != Error::Msg::NoError)
            return Error::Msg::GeneralError;
        break;
    }
    case DataBlockTypes::BdBlock:
    {
        WARNMSG("Writing data block BD is prohibited");
        break;
    }
    case DataBlockTypes::BdaBlock:
    {
        WARNMSG("Writing data block BDA is prohibited");
        break;
    }
    default:
        break;
    }
    return Error::Msg::NoError;
}

Error::Msg DataBlock::readBlockFromModule(bool update)
{
    switch (m_blockType)
    {
    case DataBlockTypes::BacBlock:
    {
        if (Commands::GetBac(m_blockNum, &m_block, m_blockSize) != Error::Msg::NoError)
            return Error::Msg::GeneralError;
        if (update)
            updateWidget();
        break;
    }
    case DataBlockTypes::BdBlock:
    {
        if (Commands::GetBd(m_blockNum, &m_block, m_blockSize) != Error::Msg::NoError)
            return Error::Msg::GeneralError;
        if (update)
            updateWidget();
        break;
    }
    case DataBlockTypes::BdaBlock:
    {
        if (Commands::GetBda(m_blockNum, &m_block, m_blockSize) != Error::Msg::NoError)
            return Error::Msg::GeneralError;
        if (update)
            updateWidget();
        break;
    }
    default:
        break;
    }
    return Error::Msg::NoError;
}

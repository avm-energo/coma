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
#include "../models/datadelegate.h"
#include "../usb/commands.h"
#include "../widgets/wd_func.h"
#include <QGroupBox>
#include <QVBoxLayout>

DataBlock::DataBlock(const BlockDescriptionStruct &bds, QWidget *parent) : QWidget(parent)
{
    m_blockDescription = bds;
    //    m_blockNum = blocknum;
    //    m_blockType = blocktype;
    //    m_block = block;
    //    m_blockSize = blocksize;
    //    m_blockName = blockname;
    m_VModel = new ValueModel;
    //    m_curModelColumn = m_curModelRow = 0;
}

void DataBlock::setModel(const QList<ValueItem *> &dd, int columnsnumber) { m_VModel->setModel(dd, columnsnumber); }

void DataBlock::setWidget()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox(m_blockDescription.blockname);
    ETableView *tv = new ETableView;
    DataDelegate *chdg = new DataDelegate;
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
    switch (m_blockDescription.blocktype)
    {
    case DataBlockTypes::BacBlock:
    {
        if (update)
            updateModel();
        if (Commands::WriteBac(m_blockDescription.blocknum, &m_blockDescription.block, m_blockDescription.blocksize)
            != Error::Msg::NoError)
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
    switch (m_blockDescription.blocktype)
    {
    case DataBlockTypes::BacBlock:
    {
        if (Commands::GetBac(m_blockDescription.blocknum, &m_blockDescription.block, m_blockDescription.blocksize)
            != Error::Msg::NoError)
            return Error::Msg::GeneralError;
        if (update)
            updateWidget();
        break;
    }
    case DataBlockTypes::BdBlock:
    {
        if (Commands::GetBd(m_blockDescription.blocknum, &m_blockDescription.block, m_blockDescription.blocksize)
            != Error::Msg::NoError)
            return Error::Msg::GeneralError;
        if (update)
            updateWidget();
        break;
    }
    case DataBlockTypes::BdaBlock:
    {
        if (Commands::GetBda(m_blockDescription.blocknum, &m_blockDescription.block, m_blockDescription.blocksize)
            != Error::Msg::NoError)
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

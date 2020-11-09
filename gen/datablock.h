#ifndef DATABLOCK_H
#define DATABLOCK_H

#include "../gen/error.h"
#include "../models/valueitem.h"
#include "../models/valuemodel.h"

#include <QWidget>

class DataBlock : public QWidget
{
    Q_OBJECT
public:
    enum DataBlockTypes
    {
        BacBlock,
        BdaBlock,
        BdBlock
    };

    struct BlockDescriptionStruct
    {
        int blocknum;                        // number of the block to send corresponding command
        QString blockname;                   // block name to set it to the GroupBox GUI
        DataBlock::DataBlockTypes blocktype; // type of the block to choose proper command
        void *block;                         // pointer to the block
        int blocksize;                       // size of the block to make a mem copy
    };

    explicit DataBlock(const BlockDescriptionStruct &bds, QWidget *parent = nullptr);
    virtual void setupUI() = 0;                                     // frontend for block visualisation
    void setModel(const QList<ValueItem *> &dd, int columnsnumber); // default columnsnumber = 5
    void setWidget();

signals:

private:
    //    QList<DataDescription> m_dataList;
    ValueModel *m_VModel;
    BlockDescriptionStruct m_blockDescription;
    //    int m_blockNum;
    //    DataBlockTypes m_blockType;
    //    QString m_blockName;
    //    void *m_block;
    //    int m_blockSize;
    //    int m_curModelRow;
    //    int m_curModelColumn;

public slots:
    void updateModel();
    void updateValues();
    Error::Msg writeBlockToModule(bool update);
    Error::Msg readBlockFromModule(bool update);
};

#endif // DATABLOCK_H

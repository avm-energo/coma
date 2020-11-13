#ifndef DATABLOCK_H
#define DATABLOCK_H

#include "../gen/error.h"
#include "../models/valuemodel.h"

#include <QWidget>

class DataBlock : public QObject
{
    Q_OBJECT
public:
    enum DataBlockTypes
    {
        BacBlock,
        BdaBlock,
        BdBlock,
        BciBlock
    };

    struct BlockStruct
    {
        int blocknum;                        // number of the block to send corresponding command
        QString caption;                     // block name to set it to the GroupBox GUI
        DataBlock::DataBlockTypes blocktype; // type of the block to choose proper command
        void *block;    // pointer to the block, for S2Config blocks it's a pointer to S2ConfigDataType
        void *defblock; // pointer to the block with default values
        int blocksize;  // size of the block to make a mem copy
    };

    struct FilePropertiesStruct
    {
        QString extension;
        QString mask;
        QString filename;
    };

    explicit DataBlock(const BlockStruct &bds, QObject *parent = nullptr);
    //    virtual void setupUI() = 0;                                     // frontend for block visualisation
    void setModel(const QList<ValueItem *> &dd, int columnsnumber); // default columnsnumber = 5
    QWidget *widget();
    BlockStruct block();
    static void getFileProperties(DataBlock::DataBlockTypes type, FilePropertiesStruct &st);

signals:

private:
    //    QList<DataDescription> m_dataList;
    ValueModel *m_VModel;
    BlockStruct m_block;
    //    int m_blockNum;
    //    DataBlockTypes m_blockType;
    //    QString m_blockName;
    //    void *m_block;
    //    int m_blockSize;
    //    int m_curModelRow;
    //    int m_curModelColumn;

public slots:
    //    void updateModel();
    //    void updateValues();
    Error::Msg writeBlockToModule();
    Error::Msg readBlockFromModule();
};

#endif // DATABLOCK_H

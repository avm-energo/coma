#ifndef DATABLOCK_H
#define DATABLOCK_H

#include "../gen/datatypes.h"
#include "../gen/error.h"
#include "../interfaces/baseinterface.h"
//#include "../models/valuemodel.h"

#include <QWidget>

class DataBlock : public QObject
{
    Q_OBJECT
public:
    struct BlockStruct
    {
        int blocknum;                        // number of the block to send corresponding command
        QString caption;                     // block name to set it to the GroupBox GUI
        DataTypes::DataBlockTypes blocktype; // type of the block to choose proper command
        void *block;   // pointer to the block, for S2Config blocks it's a pointer to S2ConfigDataType
                       //        void *defblock; // pointer to the block with default values
        int blocksize; // size of the block to make a mem copy
    };

    struct FilePropertiesStruct
    {
        QString extension;
        QString mask;
        QString filename;
    };

    explicit DataBlock(QObject *parent = nullptr);
    void setBlock(const BlockStruct &bds);
    //    virtual void setupUI() = 0;                                     // frontend for block visualisation
    //    void setModel(const QList<ValueItem *> &dd, int columnsnumber); // default columnsnumber = 5
    //    QWidget *widget();
    virtual QWidget *widget() = 0;
    BlockStruct block();
    virtual void setDefBlock() = 0;
    virtual void updateWidget() = 0;
    virtual void updateFromWidget(); // semi-virtual function, need to be reimplemented in corresponding blocks
    static void getFileProperties(DataTypes::DataBlockTypes type, FilePropertiesStruct &st);

    void readAndUpdate();

signals:

private:
    //    QList<DataDescription> m_dataList;
    //    ValueModel *m_VModel;
    BlockStruct m_block;
    //    BaseInterface *m_iface;

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

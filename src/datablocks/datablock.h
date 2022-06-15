#ifndef DATABLOCK_H
#define DATABLOCK_H

#include "../gen/datatypes.h"
#include "../gen/error.h"
#include "../interfaces/baseinterface.h"
#include "../module/board.h"
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
        //        QString filename;
    };

    QMap<DataTypes::DataBlockTypes, FilePropertiesStruct> ExtMap
        = { { DataTypes::DataBlockTypes::BacBlock, { ".tn", "Tune files (*.tn?)" } },
              { DataTypes::DataBlockTypes::BciBlock, { ".cf", "Configuration files (*.cf?)" } },
              { DataTypes::DataBlockTypes::BdBlock, { ".bd", "Data files (*.bd?)" } },
              { DataTypes::DataBlockTypes::BdaBlock, { ".bda", "Simple data files (*.bda)" } } };

    bool m_widgetIsSet;
    QWidget *m_widget;

    inline const QString cpuIDFilenameStr()
    {
        QString filenamestr
            = Board::GetInstance().UID() + ExtMap[m_block.blocktype].extension + QString::number(m_block.blocknum);
        return filenamestr;
    }
    explicit DataBlock(QObject *parent = nullptr);
    ~DataBlock();
    void setBlock(const BlockStruct &bds);
    //    virtual void setupUI() = 0;                                     // frontend for block visualisation
    //    void setModel(const QList<ValueItem *> &dd, int columnsnumber); // default columnsnumber = 5
    //    QWidget *widget();
    QWidget *widget(bool showButtons = true);
    virtual void createWidget() = 0;
    BlockStruct block();
    virtual void setDefBlock() = 0;
    virtual void updateWidget() = 0;
    virtual void updateFromWidget(); // semi-virtual function, need to be reimplemented in corresponding blocks
    //    static void getFileProperties(DataTypes::DataBlockTypes type, FilePropertiesStruct &st);
    void readBlockFromModule();
    Error::Msg writeBlockToModule();

    QWidget *blockButtonsUI();

signals:

private:
    //    QList<DataDescription> m_dataList;
    //    ValueModel *m_VModel;
    BlockStruct m_block;
    bool m_isBottomButtonsWidgetCreated;
    QWidget *m_bottomButtonsWidget;
    //    BaseInterface *m_iface;

    //    int m_blockNum;
    //    DataBlockTypes m_blockType;
    //    QString m_blockName;
    //    void *m_block;
    //    int m_blockSize;
    //    int m_curModelRow;
    //    int m_curModelColumn;

    void createBottomButtonsWidget();

public slots:
    //    void updateModel();
    //    void updateValues();
    void setDefBlockAndUpdate();
    void readAndUpdate();
    Error::Msg readFromFile();
    void readFromFileUserChoose();
    Error::Msg loadFromFileAndWriteToModule(const QString &filename);
    Error::Msg saveToFile();
    void saveToFileUserChoose();
};

#endif // DATABLOCK_H

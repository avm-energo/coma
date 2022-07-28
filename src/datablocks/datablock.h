#ifndef DATABLOCK_H
#define DATABLOCK_H

#include "../gen/datatypes.h"
#include "../gen/error.h"
#include "../interfaces/baseinterface.h"
#include "../module/board.h"

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
    };

    QMap<DataTypes::DataBlockTypes, FilePropertiesStruct> ExtMap
        = { { DataTypes::DataBlockTypes::BacBlock, { ".tn", "Tune files (*.tn?)" } },
              { DataTypes::DataBlockTypes::BciBlock, { ".cf", "Configuration files (*.cf?)" } },
              { DataTypes::DataBlockTypes::BdBlock, { ".bd", "Data files (*.bd?)" } },
              { DataTypes::DataBlockTypes::BdaBlock, { ".bda", "Simple data files (*.bda)" } } };

    inline const QString cpuIDFilenameStr()
    {
        QString filenamestr
            = Board::GetInstance().UID() + ExtMap[m_block.blocktype].extension + QString::number(m_block.blocknum);
        return filenamestr;
    }

    explicit DataBlock(QObject *parent = nullptr);
    ~DataBlock();

    /*! \brief Copy prepared block to inner variable m_block
     *  \param bds[in] - prepared block
     */
    void setBlock(const BlockStruct &bds);

    /*! \brief Returns block visualisation for insert into GUI
     *  \param showButtons[in] (bool) - show or not bottom buttons widget (load/save to/from file,
     *         load/save to/from module, set defaults - for configuration and tune parameters
     *  \returns QWidget - widget to insert into GUI
     */
    QWidget *widget(bool showButtons = true);

    /*! \brief Create block widget
     */
    virtual void createWidget() = 0;

    /*! \brief Return block
     */
    BlockStruct block();

    virtual void setDefBlock() = 0;

    /*! \brief Copy values from block to widget fields
     */
    virtual void updateWidget() = 0;

    /*! \brief Update block values from widget fields
     */
    virtual void updateFromWidget(); // semi-virtual function, need to be reimplemented in corresponding blocks

    void readBlockFromModule();
    Error::Msg writeBlockToModule();
    QWidget *blockButtonsUI();

    bool m_widgetIsSet;
    QWidget *m_widget;

signals:

private:
    BlockStruct m_block;
    bool m_isBottomButtonsWidgetCreated;
    QWidget *m_bottomButtonsWidget;

public slots:
    void setDefBlockAndUpdate();
    void readAndUpdate();
    Error::Msg readFromFile();
    void readFromFileUserChoose();
    Error::Msg loadFromFileAndWriteToModule(const QString &filename);
    Error::Msg saveToFile();
    void saveToFileUserChoose();
};

#endif // DATABLOCK_H

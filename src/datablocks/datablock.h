#ifndef DATABLOCK_H
#define DATABLOCK_H

#include "../gen/datatypes.h"
#include "../gen/error.h"
#include "../interfaces/baseinterface.h"
#include "../module/board.h"

#include <QHBoxLayout>
#include <QWidget>

class DataBlock : public QObject
{
    Q_OBJECT
public:
    static constexpr char ValuesFormat[]
        = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; font: bold; }";

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

    using ValueType = std::variant<float *, quint32 *>;

    /*! \brief Структура, описывающая значение
     */
    struct ValueStr
    {
        QString desc;
        QString tooltip;
        QString valueId;
        ValueType value;
        int precision;
    };

    struct ValueGroupStr
    {
        QString groupDesc;
        QList<ValueStr> values;
    };

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
    void createWidget();

    QHBoxLayout *addBlockValueToWidget(ValueStr &values);

    /*! \brief Return block
     */
    BlockStruct block();

    /*! \brief Copy values from block to widget fields
     */
    void updateWidget();

    /*! \brief Update specific fields
     */
    virtual void specificUpdateWidget() {};

    /*! \brief Update block values from widget fields
     */
    void updateFromWidget();

    /*! \brief Update specific values
     */
    virtual void specificUpdateFromWidget() {};

    /*! \brief Setup visual representation of block data
     */
    virtual void setupValuesDesc() = 0;

    virtual void setDefBlock() {};

    template <typename T>
    DataBlock::ValueGroupStr addGroupToValues(
        const QString &groupName, const QString &name, int howMuch, int fromWhich, T *startValue, int precision)
    {
        ValueGroupStr vg;
        vg.groupDesc = groupName;
        for (int i = 0; i < howMuch; ++i)
        {
            vg.values.append({ name + QString::number(i), "", "value[" + QString::number(i + fromWhich) + "]",
                startValue, precision });
            ++startValue;
        }
        return vg;
    }

    void readBlockFromModule();
    Error::Msg writeBlockToModule();
    QWidget *blockButtonsUI();

    bool m_widgetIsSet;
    QWidget *m_widget;
    QList<ValueGroupStr> m_valuesDesc;

signals:

private:
    BlockStruct m_block, m_defBlock;
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

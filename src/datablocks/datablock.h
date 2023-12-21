#ifndef DATABLOCK_H
#define DATABLOCK_H

#include "../module/board.h"

#include <QHBoxLayout>
#include <QScrollArea>
#include <QWidget>
#include <gen/datatypes.h>
#include <gen/error.h>
#include <variant>

class DataBlock : public QObject
{
    Q_OBJECT
public:
    static constexpr auto ValuesFormat
        = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; font: bold; }";

    struct BlockStruct
    {
        int blocknum;                        // number of the block to send corresponding command
        QString caption;                     // block name to set it to the GroupBox GUI
        DataTypes::DataBlockTypes blocktype; // type of the block to choose proper command
        void *block;   // pointer to the block, for S2Config blocks it's a pointer to S2ConfigDataType
                       //        void *defblock; // pointer to the block with default values
        int blocksize; // size of the block to make a mem copy
        bool bottomButtonsVisible;
    };

    struct FilePropertiesStruct
    {
        QString extension;
        QString mask;
    };

    QMap<DataTypes::DataBlockTypes, FilePropertiesStruct> ExtMap = {
        { DataTypes::DataBlockTypes::BacBlock, { ".tn", "Tune files (*.tn?)" } },          //
        { DataTypes::DataBlockTypes::BciBlock, { ".cf", "Configuration files (*.cf?)" } }, //
        { DataTypes::DataBlockTypes::BdBlock, { ".bd", "Data files (*.bd?)" } },           //
        { DataTypes::DataBlockTypes::BdaBlock, { ".bda", "Simple data files (*.bda)" } }   //
    };

    using ValueType = std::variant<float *, quint32 *>;

    /// \brief Структура, описывающая значение
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

    /// \brief Copy prepared block to inner variable m_block
    /// \param bds[in] - prepared block
    void setBlock(const BlockStruct &bds);

    /// \brief Returns block visualisation for insert into GUI
    /// \param showButtons[in] (bool) - show or not bottom buttons widget (load/save to/from file,
    ///         load/save to/from module, set defaults - for configuration and tune parameters
    /// \returns QWidget - widget to insert into GUI
    QWidget *widget(bool showButtons = true);

    /// \brief Create block widget
    void createWidget();

    QHBoxLayout *addBlockValueToWidget(ValueStr &values, QWidget *parent = nullptr);

    /// \brief Return block
    BlockStruct block();

    /// \brief Copy values from block to widget fields
    void updateWidget();

    /// \brief Update specific fields
    virtual void specificUpdateWidget() {};

    /// \brief Update block values from widget fields
    void updateFromWidget();

    /// \brief Update specific values
    virtual void specificUpdateFromWidget() {};

    /// \brief Setup visual representation of block data
    virtual void setupValuesDesc() = 0;

    virtual void setDefBlock() {};

    template <typename T>                                           //
    void addNewGroup(const QString &groupName, const QString &name, //
        int fromWhich, int howMuch, T *startValue, int precision)
    {
        ValueGroupStr vg;
        vg.groupDesc = groupName;
        int start = fromWhich;
        int end = fromWhich + howMuch;
        for (int i = start; i < end; ++i)
        {
            vg.values.append({ name + QString::number(i), "", "value[" + QString::number(valueNumberCounter) + "]",
                startValue, precision });
            ++startValue;
            ++valueNumberCounter;
        }
        m_valuesDesc.append(vg);
    }

    template <typename T> //
    void addNewValue(const QString &name, const QString &tooltip, T *value, int precision = 3)
    {
        ValueGroupStr vg;
        vg.values.append({ name, tooltip, "value[" + QString::number(valueNumberCounter) + "]", value, precision });
        ++valueNumberCounter;
        m_valuesDesc.append(vg);
    }

    void readBlockFromModule();
    Error::Msg writeBlockToModule();
    QWidget *blockButtonsUI();
    void setEnabled(bool isEnabled);

public slots:
    void setDefBlockAndUpdate();
    void readAndUpdate();
    Error::Msg readFromFile();
    void readFromFileUserChoose();
    Error::Msg loadFromFileAndWriteToModule(const QString &filename);
    Error::Msg saveToFile();
    void saveToFileUserChoose();

private:
    bool m_widgetIsSet;
    QWidget *m_widget;
    int valueNumberCounter;
    BlockStruct m_block, m_defBlock;
    bool m_isBottomButtonsWidgetCreated;
    QWidget *m_bottomButtonsWidget;
    QList<ValueGroupStr> m_valuesDesc;
};

#endif // DATABLOCK_H

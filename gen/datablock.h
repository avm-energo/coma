#ifndef DATABLOCK_H
#define DATABLOCK_H

#include "../gen/error.h"
#include "../models/valuemodel.h"
#include <QWidget>

#define MAX_VALUEMODEL_COLUMNS 5

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

    enum DataStyles
    {
        Label,
        LineEdit,
        SpinBox,
        ComboBox
    };

    enum DataFormat
    {
        SIMPLE_CELL = 0, // general data output
        OUTVALUEINT = 1, // int
        OUTVALUEFLOAT0 = 2, // 0 digits after zero
        OUTVALUEFLOAT1 = 3,
        OUTVALUEFLOAT2 = 4,
        OUTVALUEFLOAT3 = 5,
        OUTVALUEFLOAT4 = 6,
        OUTVALUEFLOAT5 = 7, // 5 digits after zero
        OUTVALUESTRING = 8, // abracadabra
        OUTVALUEDOUBLE = 9, // 16e-018
        OUTVALUEHEX = 10 // 0x5af3
    };

    struct DataDescription
    {
        void *dataptr;
        QString name;
        QString tooltip;
        DataStyles style;
        DataFormat format;
        QObject *adddata; // for ComboBoxes - QStringList *, for SpinBoxes - SpbDataStyleAdditionalDataClass *
    };

    class SpbDataStyleAdditionalDataClass : public QObject
    {
    public:
        double min;
        double max;
        int decimals;
    };

    explicit DataBlock(void *block, const QString &blockname, int blocksize, DataBlockTypes blocktype, int blocknum,
        QWidget *parent = nullptr);
    virtual void setupUI() = 0; // frontend for block visualisation
    void addNewItem(const DataDescription &dd);
    void setWidget();

signals:

private:
    QList<DataDescription> m_dataList;
    ValueModel *m_VModel;
    int m_blockNum;
    DataBlockTypes m_blockType;
    QString m_blockName;
    void *m_block;
    int m_blockSize;
    int m_curModelRow;
    int m_curModelColumn;

public slots:
    void updateModel();
    void updateWidget();
    Error::Msg writeBlockToModule(bool update);
    Error::Msg readBlockFromModule(bool update);
};

#endif // DATABLOCK_H

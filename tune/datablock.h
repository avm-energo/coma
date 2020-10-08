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

    struct DataDescription
    {
        void *dataptr;
        QString name;
        QString tooltip;
    };

    explicit DataBlock(void *block, const QString &blockname, int blocksize, DataBlockTypes blocktype, int blocknum,
        QWidget *parent = nullptr);
    virtual void setupUI() = 0; // frontend for block visualisation
    void addNewItem(const QString &name, void *ptr, const QString &tooltip = "");
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

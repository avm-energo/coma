#ifndef OSCDIALOG_H
#define OSCDIALOG_H

#include "../models/etablemodel.h"
#include "../widgets/udialog.h"
#include "oscmanager.h"

#include <optional>

class OscDialog : public UDialog
{
    Q_OBJECT
    using OscHeader = S2DataTypes::OscHeader;
    enum Column
    {
        number = 0,
        datetime = 1,
        id = 2,
        size = 3,
        download = 4
    };

public:
    explicit OscDialog(QWidget *parent = nullptr);

public slots:
    void fillOscInfo(S2DataTypes::OscInfo info);
    void fillOsc(const DataTypes::FileStruct file);

private:
    void setupUI();
    void getOsc(const QModelIndex &);
    void eraseOsc();
    QString filename(quint64 time, quint32 oscNum) const;
    bool loadIfExist(quint32 size);

    OscManager manager;
    std::unique_ptr<TrendViewModel> oscModel;
    QMap<int, S2DataTypes::OscInfo> oscMap;
    std::vector<DataTypes::FileStruct> fileBuffer;
    UniquePointer<ETableModel> tableModel;
    int reqOscNum;
};

#endif // OSCDIALOG_H

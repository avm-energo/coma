#pragma once

#include "../widgets/udialog.h"

#include <models/etablemodel.h>
#include <optional>
#include <oscillograms/oscmanager.h>

class OscDialog : public UDialog
{
    Q_OBJECT
    using OscHeader = S2::OscHeader;
    enum Column
    {
        number = 0,
        datetime = 1,
        id = 2,
        size = 3,
        download = 4
    };

public:
    explicit OscDialog(Device::CurrentDevice *device, QWidget *parent = nullptr);

public slots:
    void fillOscInfo(const S2::OscInfo &info);
    void fillOsc(const S2::FileStruct &file);

private:
    void setupUI();
    void getOsc(const QModelIndex &);
    void eraseOsc();
    QString filename(quint64 time, quint32 oscNum) const;
    bool loadIfExist(quint32 size);

    OscManager manager;
    std::unique_ptr<TrendViewModel> oscModel;
    QMap<int, S2::OscInfo> oscMap;
    std::vector<S2::FileStruct> fileBuffer;
    UniquePointer<ETableModel> tableModel;
    int reqOscNum;
};

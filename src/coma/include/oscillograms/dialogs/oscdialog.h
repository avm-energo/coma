#pragma once

#include <comawidgets/udialog.h>
#include <models/etablemodel.h>
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
    void disableResponseConnections() override;
    void enableResponseConnections() override;

public slots:
    void fillOscInfo(const S2::OscInfo &info);
    void fillOsc(const S2::FileStruct &file);

private:
    void setupUI();
    void getOsc(const QModelIndex &);
    void eraseOsc();
    QString filename(quint64 time, quint32 oscNum) const;
    bool loadIfExist(quint32 size);

    OscManager m_manager;
    QMap<int, S2::OscInfo> m_oscMap;
    std::vector<S2::FileStruct> m_fileBuffer;
    UniquePointer<ETableModel> m_tableModel;
    int m_reqOscNum;
    QMetaObject::Connection m_fillOscConnection, m_fillOscInfoConnection;
};

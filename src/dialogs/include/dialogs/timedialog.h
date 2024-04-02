#pragma once

#include "../../widgets/udialog.h"

class TimeDialog final : public UDialog
{
    Q_OBJECT

public:
    explicit TimeDialog(Device::CurrentDevice *device, QWidget *parent = nullptr);
    ~TimeDialog();

private:
    void setupUI();
    void setTime(quint32 unixtimestamp);
    void writeTime(QDateTime &myDateTime);

    bool First;
    QTimer *Timer;

public slots:
    void errorRead();
    void reqUpdate();

private slots:

    void writePCDate();
    void writeDate();
    void uponInterfaceSetting();
    void updateBitStringData(const DataTypes::BitStringStruct &bs);
};

namespace settings
{
enum Widgets : int
{
    Timezone,
    DesktopDatetime,
    ModuleDatetime,
    ModuleDatetimeUtc,
    WriteDatetime
};

inline QString nameByValue(Widgets key)
{
    return QString::number(key);
}
}

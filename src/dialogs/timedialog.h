#ifndef MNKTIME_H
#define MNKTIME_H

#include "../widgets/udialog.h"

class TimeDialog : public UDialog
{
    Q_OBJECT
    enum TimeZone : int
    {
        DesktopTZ = 0,
        UTC = 1,

    };
    Q_PROPERTY(TimeZone timeZone READ timeZone WRITE setTimeZone NOTIFY timeZoneChanged)
public:
    explicit TimeDialog(QWidget *parent = nullptr);
    ~TimeDialog();

    TimeZone timeZone() const;
    void setTimeZone(int timeZone);

private:
    void setupUI();
    void setTime(quint32 unixtimestamp);
    void writeTime(QDateTime &myDateTime);

    bool First;
    QTimer *Timer;
    TimeZone m_timeZone;

signals:

    void timeZoneChanged(TimeDialog::TimeZone timeZone);

public slots:
    void errorRead();
    void reqUpdate();

private slots:

    void writePCDate();
    void writeDate();
    void uponInterfaceSetting();
    void updateBitStringData(const DataTypes::BitStringStruct &bs);
};

namespace detail
{
enum Widgets : int
{
    Timezone,
    DesktopDatetime,
    ModuleDatetime,
    WriteDatetime
};

inline QString nameByValue(Widgets key)
{
    return QString::number(key);
}
}

#endif // MNKTIME_H

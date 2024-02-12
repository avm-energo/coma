#ifndef UWIDGET_H
#define UWIDGET_H

#include <QWidget>
#include <interfaces/moduledataupdater.h>

namespace Device
{
class CurrentDevice;
} // namespace Device

class UWidget : public QWidget
{
    Q_OBJECT
public:
    struct HighlightWarnAlarmStruct
    {
        quint32 fieldnum;
        QString color;
    };

    explicit UWidget(Device::CurrentDevice *device, QWidget *parent = nullptr);
    bool updatesEnabled();
    const QString getCaption();
    void setCaption(const QString &caption);
    virtual void uponInterfaceSetting();
    virtual void reqUpdate();
    virtual void updateFloatData(const DataTypes::FloatStruct &fl);
    virtual void updateSPData(const DataTypes::SinglePointWithTimeStruct &sp);
    virtual void updateBitStringData(const DataTypes::BitStringStruct &bs);
    ModuleDataUpdater *engine();
    Device::CurrentDevice *device();

protected:
    bool checkPassword();
    ModuleDataUpdater *m_dataUpdater;
    Device::CurrentDevice *m_device;
};

#endif // UWIDGET_H

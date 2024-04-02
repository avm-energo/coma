#pragma once

#include "../../widgets/udialog.h"

#include <QHBoxLayout>
#include <QLabel>
#include <device/xml_settings.h>

/// \brief Абстрактный класс проверки работы модулей АВ-ТУК
class CheckDialog final : public UDialog
{
    Q_OBJECT
public:
    using HighlightMap = QMultiMap<quint32, quint32>;

    explicit CheckDialog(
        const Device::XmlDataTypes::Section &mSection, Device::CurrentDevice *device, QWidget *parent = nullptr);
    ~CheckDialog() override;
    void setHighlights(Device::XmlDataTypes::AlarmType type, const HighlightMap &map);

public slots:
    void reqUpdate() override;

private:
    struct BdBlocks
    {
        void *block;
        int blocknum;
        bool toxlsxwrite;
    };

    struct TabWidget
    {
        QString widgetCaption;
        UWidget *widget;
    };

    /*! \brief QList для вкладок текущего виджета
     *  \details QList для вкладок текущего виджета, по умолчанию обновление отключено
     *  для всех вкладок, если хотим обновлять какую-то вкладку сразу, то
     *  необходимо включить обновление для нее
     */
    QList<TabWidget> m_TabList;
    QMap<int, BdBlocks *> Bd_blocks;
    const Device::XmlDataTypes::Section &mSection;
    int currentTabIndex;
    HighlightMap m_highlightWarn, m_highlightCrit;

    void setupUI();
    void setupTabWidget();
    void addSignals(const QList<Device::XmlDataTypes::SGroup> &groups, UWidget *widget);
    void uponInterfaceSetting() override;
    void updateSPData(const DataTypes::SinglePointWithTimeStruct &sp) override;
    QString getFormatted(const Device::XmlDataTypes::MWidget &widget, //
        const QString &form, const quint32 number, const quint32 start = 1);
    void updatePixmap(const Device::XmlDataTypes::MWidget &mwidget, //
        const DataTypes::BitStringStruct &bs, UWidget *uwidget);
    QLabel *createPixmapIndicator(const Device::XmlDataTypes::MWidget &mwidget, const quint32 index);
    QVBoxLayout *setupGroup(const Device::XmlDataTypes::SGroup &arg, UWidget *uwidget);
    QGridLayout *setupFloatWidget(const Device::XmlDataTypes::MWidget &mwidget, const int wCount);
    QVBoxLayout *setupBitsetWidget(const Device::XmlDataTypes::MWidget &mwidget, UWidget *dataUpdater);

private slots:
    void tabChanged(int newIndex);
};

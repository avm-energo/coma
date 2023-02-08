#pragma once
#include "../module/modulesettings.h"
#include "../widgets/udialog.h"

#include <QHBoxLayout>
#include <QLabel>

/// \brief Абстрактный класс проверки работы модулей АВ-ТУК
class CheckDialog : public UDialog
{
    Q_OBJECT
public:
    using HighlightMap = QMultiMap<quint32, quint32>;
    explicit CheckDialog(const ModuleTypes::Section &mSection, QWidget *parent = nullptr);
    ~CheckDialog() override;
    void setHighlights(Modules::AlarmType type, const HighlightMap &map);

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
    const ModuleTypes::Section &mSection;
    int currentTabIndex;
    HighlightMap m_highlightWarn, m_highlightCrit;

    void setupUI();
    void setupTabWidget();
    void addSignals(const QList<ModuleTypes::SGroup> &groups, UWidget *widget);
    void uponInterfaceSetting() override;
    void updateSPData(const DataTypes::SinglePointWithTimeStruct &sp) override;
    QString getFormatted(const ModuleTypes::MWidget &widget, const QString &form, const int &number);
    void updatePixmap(const ModuleTypes::MWidget &mwidget, DataTypes::BitStringStruct &bs, UWidget *uwidget);
    QLabel *createPixmapIndicator(const ModuleTypes::MWidget &mwidget, const quint32 &index);
    QVBoxLayout *setupGroup(const ModuleTypes::SGroup &arg, UWidget *uwidget);
    QGridLayout *setupFloatWidget(const ModuleTypes::MWidget &mwidget, const int &wCount);
    QVBoxLayout *setupBitsetWidget(const ModuleTypes::MWidget &mwidget, UWidget *dataUpdater);

private slots:
    void tabChanged(int newIndex);
};

#pragma once

#include <comawidgets/udialog.h>
#include <comawidgets/widgetfactory.h>
#include <dialogs/dialogcreator.h>
#include <dialogs/errconfstate.h>
#include <s2/s2datamanager.h>

#include <set>

class QTabWidget;

class ConfigDialog final : public UDialog
{
public:
    explicit ConfigDialog(Device::CurrentDevice *device, const S2BoardType boardType, QWidget *parent = nullptr);
    void setDefaultConfig();

private slots:
    void parseStatusHandle(const Error::Msg status);
    void noConfigurationHandle();
    void fillBack();

private:
    struct OrderedWidget
    {
        QWidget *widget;
        u16 order;
    };
    using WidgetList = QList<OrderedWidget>;
    QWidget *ConfButtons();
    u32 tabForId(u32 id);
    std::set<u32> createTabs(QTabWidget *tabWidget);
    void setupUI();

    void fill();
    bool prepareConfigToWrite();
    void uponInterfaceSetting() override;
    void checkConfig();
    void saveConfigToFile();
    void loadConfigFromFile();
    void readConfig();
    void writeConfig();
    void checkForDiff();
    bool isVisible(const u32 id) const;
    bool isDebugWidget(const u32 id) const;
    void showConfigErrState();
    void insertWidgetIntoListByItsOrder(const u16 id, QWidget *w, WidgetList &wlist);

    S2DataManager &m_datamanager;
    S2BoardConfig &m_boardConfig;
    WidgetFactory m_factory;
    ErrConfState *m_errConfState;
    QStringList m_confErrors;
};

#pragma once

#include "../module/modules.h"
#include "../module/modulesettings.h"
#include "../widgets/udialog.h"

#include <map>
#include <set>

class QBoxLayout;
class QGroupBox;
class QPaintEvent;
class QTabWidget;

class NewHiddenDialog final : public UDialog
{
    Q_OBJECT
private:
    ModuleTypes::HiddenSettings m_settings;
    std::map<quint32, bool> m_srcAddrStates;
    QString m_currentBackground;
    bool m_isGodMode;

    void generateDefaultSettings();
    void prepareInternalData(const ModuleTypes::SignalMap &sigMap);

    void setupUI();
    QGroupBox *setupGroupBox(const ModuleTypes::HiddenTab &hiddenTab);
    void updateUI();
    void updateWidget(const bool enabled, const ModuleTypes::HiddenWidget &widget);
    void paintEvent(QPaintEvent *e) override;

    const ModuleTypes::HiddenWidget *findWidgetByAddress(const quint32 addr) const noexcept;
    void verifyFilling() noexcept;
    void updateBitStringData(const DataTypes::BitStringStruct &bs) override;
    void fillWidget(const quint32 value, const ModuleTypes::HiddenWidget &widgetData);

public:
    explicit NewHiddenDialog(const ModuleSettings &settings, QWidget *parent = nullptr);

    void fill();
    void fillBack();
    void setModuleName(const QString &moduleName);

signals:
    void fillingFinished();
};

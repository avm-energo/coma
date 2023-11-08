#pragma once

#include "../module/modules.h"
#include "../module/modulesettings.h"
#include "../widgets/udialog.h"

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
    std::vector<ModuleTypes::Signal> m_requestSignals;
    std::set<quint32> m_srcAddresses;
    QString m_currentBackground;

    void generateDefaultSettings();
    void prepareInternalData(const ModuleTypes::SignalMap &sigMap);

    void setupUI();
    QGroupBox *setupGroupBox(const ModuleTypes::HiddenTab &hiddenTab);
    void paintEvent(QPaintEvent *e) override;

    const ModuleTypes::HiddenWidget *findWidgetByAddress(const quint32 addr) const noexcept;
    void updateBitStringData(const DataTypes::BitStringStruct &bs) override;
    void fillWidget(const quint32 value, const ModuleTypes::HiddenWidget &widget);

public:
    explicit NewHiddenDialog(const ModuleSettings &settings, QWidget *parent = nullptr);

    void fill();
    void fillBack();
};

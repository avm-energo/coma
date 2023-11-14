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

class HiddenDialog final : public UDialog
{
    Q_OBJECT
private:
    ModuleTypes::HiddenSettings m_settings;
    std::map<quint32, bool> m_srcAddrStates;
    QString m_currentBackground;
    DataTypes::HardwareStruct m_hardwareInfo;
    bool m_isGodMode, m_isAlreadyFilled;
    bool m_isSendedEnableCmd, m_isSendedWritingCmd;

    void generateDefaultSettings();
    void prepareInternalData(const ModuleTypes::SignalMap &sigMap);

    void setupUI();
    QGroupBox *setupGroupBox(const ModuleTypes::HiddenTab &hiddenTab);
    void updateUI();
    bool isTabEnabled(const ModuleTypes::HiddenTab &tabSettings) const noexcept;
    void updateWidget(const bool enabled, const ModuleTypes::HiddenWidget &widget);
    void paintEvent(QPaintEvent *e) override;

    const ModuleTypes::HiddenWidget *findWidgetByAddress(const quint32 addr) const noexcept;
    void verifyFilling() noexcept;
    void updateBitStringData(const DataTypes::BitStringStruct &bs) override;
    void fillWidget(const quint32 value, const ModuleTypes::HiddenWidget &widgetData);

    quint32 fillBackWidget(const ModuleTypes::HiddenWidget &widgetData);
    void updateGeneralResponse(const DataTypes::GeneralResponseStruct &response) override;

public:
    explicit HiddenDialog(const ModuleSettings &settings, QWidget *parent = nullptr);
    void setModuleName(const QString &moduleName);

public slots:
    void fill();
    void fillBack();
};

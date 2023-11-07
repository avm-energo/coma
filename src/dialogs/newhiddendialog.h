#pragma once

#include "../module/modules.h"
#include "../module/modulesettings.h"
#include "../widgets/udialog.h"

class QPaintEvent;
class QTabWidget;
class QBoxLayout;

class NewHiddenDialog final : public UDialog
{
    Q_OBJECT
private:
    ModuleTypes::HiddenSettings m_settings;
    QString m_currentBackground;

    void generateDefaultSettings();
    void setupUI();
    void setupGroupBox(const ModuleTypes::HiddenTab &hiddenTab);

    void fill();
    void fillBack();

protected:
    void paintEvent(QPaintEvent *e);

public:
    explicit NewHiddenDialog(const ModuleTypes::HiddenSettings &settings, QWidget *parent = nullptr);
};

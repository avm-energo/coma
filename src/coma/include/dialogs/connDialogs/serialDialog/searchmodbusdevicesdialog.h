#pragma once

#include <array>
#include <dialogs/connDialogs/serialDialog/searchproccessdialog.h>

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QGroupBox;
class QHBoxLayout;
class QStackedWidget;
class SearchProccessDialog;

class SearchModbusDevicesDialog final : public QWidget
{
private:
    struct
    {
        QComboBox *port;
        QDoubleSpinBox *timeout;
        QDoubleSpinBox *startAddr;
        QDoubleSpinBox *endAddr;
        std::array<QCheckBox *, 7> baud;
        QCheckBox *parityNone, *parityOdd, *parityEven;
        QCheckBox *stopBitOne, *stopBitTwo;
    } m_widgets;

    SearchParams m_data;
    InterfaceSerialDialog *m_targetDialog {nullptr};

    /// \brief Switches between the search-parameters form and the running search's process view.
    QStackedWidget *m_stack {nullptr};
    /// \brief The search-parameters form; shown again once the process view is cancelled.
    QWidget *m_paramsWidget {nullptr};

    QGroupBox *createComGroupBox();
    QGroupBox *createTimeoutGroupBox();
    QGroupBox *createAddressGroupBox();
    QGroupBox *createBaudGroupBox();
    QGroupBox *createParityGroupBox();
    QGroupBox *createStopbitsGroupBox();
    QHBoxLayout *createControlLayout();
    void setupUI();

    void getData();
    bool validate();
    void runSearch();

public:
    SearchModbusDevicesDialog(InterfaceSerialDialog *targetDialog, QWidget *parent = nullptr);
};

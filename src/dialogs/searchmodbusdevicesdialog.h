#pragma once

#include <QDialog>
#include <array>

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QGroupBox;
class QHBoxLayout;

class SearchModbusDevicesDialog : public QDialog
{
private:
    struct
    {
        QComboBox *com;
        QDoubleSpinBox *timeout;
        QDoubleSpinBox *startAddr;
        QDoubleSpinBox *endAddr;
        std::array<QCheckBox *, 7> baud;
        std::array<QCheckBox *, 3> parity;
        std::array<QCheckBox *, 2> stopBits;
    } widgets;

    QGroupBox *createComGroupBox();
    QGroupBox *createTimeoutGroupBox();
    QGroupBox *createAddressGroupBox();
    QGroupBox *createBaudGroupBox();
    QGroupBox *createParityGroupBox();
    QGroupBox *createStopbitsGroupBox();
    QHBoxLayout *createControlLayout();
    void setupUI();
    void runSearch();

public:
    SearchModbusDevicesDialog(QWidget *parent = nullptr);
};

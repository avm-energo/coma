#pragma once

#include "searchproccessdialog.h"

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
        QComboBox *port;
        QDoubleSpinBox *timeout;
        QDoubleSpinBox *startAddr;
        QDoubleSpinBox *endAddr;
        std::array<QCheckBox *, 7> baud;
        QCheckBox *parityNone, *parityOdd, *parityEven;
        QCheckBox *stopBitOne, *stopBitTwo;
    } widgets;

    SearchParams data;

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
    SearchModbusDevicesDialog(QWidget *parent = nullptr);
};

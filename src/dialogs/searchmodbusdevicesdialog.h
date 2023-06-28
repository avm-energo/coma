#pragma once

#include <QDialog>
#include <memory>

struct SearchDialogWidgets;
class QGroupBox;

class SearchModbusDevicesDialog : public QDialog
{
private:
    std::unique_ptr<SearchDialogWidgets> widgets;

    void selectAllCheckBoxes();
    void clearAllCheckBoxes();
    QGroupBox *createComGroupBox();
    QGroupBox *createTimeoutGroupBox();
    QGroupBox *createBaudGroupBox();
    void setupUI();

public:
    SearchModbusDevicesDialog(QWidget *parent = nullptr);
};

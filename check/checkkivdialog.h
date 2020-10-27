#ifndef CHECKDIALOGKIV_H
#define CHECKDIALOGKIV_H

#include "../interfaces/baseinterface.h"
#include "abstractcheckdialog.h"
#include "check.h"
#include "checkkiv.h"

class CheckKIVDialog : public AbstractCheckDialog
{
    Q_OBJECT
public:
    explicit CheckKIVDialog(QWidget *parent = nullptr);
    ~CheckKIVDialog();

public slots:
    void SetAlarmColor(int position, bool value) override;
    void SetWarnColor(int position, bool value) override;
    void getFloatData();
    void getSPData();
    void updateFloatData();
    void updateSPData();
    //    void MainTWTabClicked(int tabindex);
    void update() override;

private:
    CheckKIV *ChKIV;
    QBitArray m_stColor;

    QWidget *AutoCheckUI();            // UI для автоматической проверки модуля
    QWidget *BdUI(int bdnum) override; // визуализация наборов текущих данных от модуля
    void PrepareHeadersForFile(int row) override;  // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum) override; // row - номер строки для записи в файл
                                                   // xlsx, bdnum - номер блока данных
    void ChooseValuesToWrite() override;
    void SetDefaultValuesToWrite() override;
    void PrepareAnalogMeasurements() override;
    //    QWidget *CustomTab() override;

private slots:
    //    void UpdateModBusData(QList<ModBus::SignalStruct> Signal) override;
};
#endif // CHECKDIALOG84_H

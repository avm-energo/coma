#ifndef CHECKDIALOGKDV_H
#define CHECKDIALOGKDV_H

#include "abstractcheckdialog.h"
#include "checkkdv.h"

class CheckKDVDialog : public AbstractCheckDialog
{
    Q_OBJECT
public:
    explicit CheckKDVDialog(QWidget *parent = nullptr);

    QWidget *EParent;
public slots:
    //    void SetWarnColor(int position, bool value) override;
    //    void SetAlarmColor(int position, bool value) override;
    //    void updateFloatData(DataTypes::FloatStruct &floatsignal);
    //    void USBUpdate() override;
    //    void ETHUpdate() override;
    //    void MBSUpdate() override;

private:
    CheckKDV *ChKDV;
    //    QWidget *AutoCheckUI();            // UI для автоматической проверки модуля
    //    QWidget *BdUI(int bdnum) override; // визуализация наборов текущих данных от модуля
    void PrepareHeadersForFile(int row) override;  // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum) override; // row - номер строки для записи в файл
                                                   // xlsx, bdnum - номер блока данных
                                                   //    void ChooseValuesToWrite() override;
                                                   //    void SetDefaultValuesToWrite() override;
                                                   //    void PrepareAnalogMeasurements() override;

    UWidget *Bd1W();
    UWidget *Bd2W();
    UWidget *Bd3W();
    UWidget *Bd4W();
    UWidget *Bd5W();
    UWidget *Bd6W();
    UWidget *Bd7W();
    UWidget *Bd8W();

    //    QWidget *CustomTab() override;
    //    void setConnections() override;

private slots:
    //    void UpdateModBusData(QList<ModBus::SignalStruct> Signal) override;
};

#endif // CHECKDIALOGKDV_H
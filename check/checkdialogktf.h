#ifndef CHEKDIALOGKTF_H
#define CHEKDIALOGKTF_H

#include "../iec104/iec104.h"
#include "checkktf.h"
#include "eabstractcheckdialog.h"

class CheckDialogKTF : public EAbstractCheckDialog
{
    Q_OBJECT

public:
    CheckDialogKTF(BoardTypes board = BoardTypes::BT_BASE, QWidget *parent = nullptr);
    //    CheckDialogKTF(int rcount = 4, BoardTypes board = BoardTypes::BT_BASE, QWidget *parent = nullptr);

public slots:
    void SetPredAlarmColor(quint8 *);
    void SetAlarmColor(quint8 *Alarm);
    void UpdateFlData(IEC104Thread::FlSignals104 *);
    void UpdateSponData(IEC104Thread::SponSignals *);
    void USBUpdate() override;

private:
    CheckKTF *ChKTF;
    // CheckHarmonicKTF *ChHarmKTF;
    // int m_RCount;

    QWidget *AutoCheckUI();            // UI для автоматической проверки модуля
    QWidget *BdUI(int bdnum) override; // визуализация наборов текущих данных от модуля
    void RefreshAnalogValues(int bdnum) override;  // обновление полей в GUI из полученного
                                                   // соответствующего Bd_block
    void PrepareHeadersForFile(int row) override;  // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum) override; // row - номер строки для записи в файл
                                                   // xlsx, bdnum - номер блока данных
    void ChooseValuesToWrite() override;
    void SetDefaultValuesToWrite() override;
    void PrepareAnalogMeasurements() override;
    QWidget *CustomTab() override;

private slots:
    void UpdateModBusData(QList<ModBus::SignalStruct> Signal) override;
};

#endif // CHEKDIALOGKTF_H

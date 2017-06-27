#ifndef ABSTRACTCHECKDIALOG_H
#define ABSTRACTCHECKDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QtXlsx/xlsxdocument.h>

#define ANMEASINT   2000 // default timer interval to check analog values

class AbstractCheckDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AbstractCheckDialog(QWidget *parent = 0);

    virtual QWidget *AutoCheckUI(); // UI для автоматической проверки модуля
    virtual QWidget *Bd1UI(); // первый набор текущих данных от модуля
    virtual QWidget *Bd2UI(); // второй набор текущих данных от модуля
    virtual QWidget *Bd3UI(); // третий набор текущих данных от модуля
    virtual void RefreshAnalogValues();
    virtual void ReadAnalogMeasurements();
    virtual void PrepareHeadersForFile(int row); // row - строка для записи заголовков

signals:

public slots:

private:
    struct Bip
    {
        quint8 ip[4];
    };

    Bip Bip_block;
    QTimer *timer;
    QXlsx::Document *xlsx;
    bool XlsxWriting;
    int WRow;
    QTime *ElapsedTimeCounter;

    void SetupUI();
    void CheckIP();
    void GetIP();

private slots:
    void StartAnalogMeasurementsToFile();
    void StartAnalogMeasurements();
    void StopAnalogMeasurements();
    void SetTimerPeriod();
    void TimerTimeout();
};

#endif // ABSTRACTCHECKDIALOG_H

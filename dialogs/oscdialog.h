#ifndef OSCDIALOG_H
#define OSCDIALOG_H

#include <QDialog>
#include <QByteArray>

#include "../gen/eoscillogram.h"
#include "../widgets/etablemodel.h"

#define MT_FT_XLSX      0x01
#define MT_FT_COMTRADE  0x02
#define MT_FT_NONE      0x04

class OscDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OscDialog(EOscillogram *osc, QWidget *parent=nullptr);
    ~OscDialog();

private:
    void SetupUI();
    void UpdateModel();

    ETableModel *tm;
    EOscillogram *OscFunc;

signals:
    void StopCheckTimer();

public slots:

private slots:
#if PROGSIZE != PROGSIZE_EMUL
    void GetAndProcessOscInfo();
    void GetOsc(QModelIndex);
    void EraseOsc();
#endif
};

#endif // OSCDIALOG_H

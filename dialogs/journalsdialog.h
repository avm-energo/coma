#ifndef JOURNALDIALOG_H
#define JOURNALDIALOG_H

#include <QDialog>
#include <QModelIndex>
#include <QThread>

#include "QtXlsx/xlsxdocument.h"
#include "../gen/maindef.h"
#include "../widgets/etableview.h"
#include "../models/etablemodel.h"
#include "../gen/s2.h"
#include "../dialogs/keypressdialog.h"
#include "../gen/journals.h"
#include "../widgets/waitwidget.h"
#include "../iec104/iec104.h"

#define MAXSWJNUM   262144

class JournalDialog : public QDialog
{
    Q_OBJECT
public:
    JournalDialog(IEC104 *iec, QWidget *parent = nullptr);
    ~JournalDialog();

    quint8 start;
    int SaveI;

private:
    WaitWidget *WW;
    QThread JourThread;

    void SetupUI();
    QWidget *JourTab(int jourtype);
    int GetJourNum(const QString &objname);

signals:
    void WritePasswordChecked();

private slots:
    void TryGetJourByUSB();
    void GetJour();
    void JourFileChoosed(QString &file);
    void EraseJour();
    void SaveJour();
    void WritePasswordCheck(QString psw);
    void Done();
    void Error(QString msg);
    void Ready(ETableModel *mdl);

public slots:

private:
    QString JourFile;
    int JourType;
    bool ok;
    Journals *JourFuncs;

    int WriteCheckPassword();
    void StartReadJourFile();
};

#endif // JOURNALDIALOG_H

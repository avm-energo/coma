#ifndef JOURNALDIALOG_H
#define JOURNALDIALOG_H

#include <QDialog>
#include <QModelIndex>
#include <QThread>

#include "../dialogs/keypressdialog.h"
#include "../gen/journals.h"
#include "../gen/maindef.h"
#include "../gen/s2.h"
#include "../iec104/iec104.h"
#include "../models/etablemodel.h"
#include "../widgets/etableview.h"
#include "../widgets/waitwidget.h"

#define MAXSWJNUM 262144

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
    void StartGetJour(); // start fet journal thread
    void StartReadFile();
    void StartSaveJour(int, QAbstractItemModel *, QString);

private slots:
    void TryGetJourByUSB();
    void GetJour();
    void JourFileChoosed(QString &file);
    void EraseJour();
    void SaveJour();
    void WritePasswordCheck(QString psw);
    void Done(QString msg);
    void Error(QString msg);
    void SetModel(ETableModel *mdl);
//    void SetProxyModel(QSortFilterProxyModel *mdl);

public slots:

private:
    QString JourFile;
    int JourType;
    bool ok;
    Journals *JourFuncs;
    QSortFilterProxyModel *ProxyWorkModel, *ProxySysModel, *ProxyMeasModel;

    int WriteCheckPassword();
    void StartReadJourFile();
};

#endif // JOURNALDIALOG_H

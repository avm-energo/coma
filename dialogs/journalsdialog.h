#ifndef JOURNALDIALOG_H
#define JOURNALDIALOG_H

#include "../gen/journals.h"
#include "../iec104/iec104.h"

#include <QDialog>
#include <QMessageBox>
#include <QProgressDialog>

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
    QMessageBox *MsgBox;
    QProgressDialog *progress;

    void SetupUI();
    QWidget *JourTab(int jourtype);
    int GetJourNum(const QString &objname);

signals:
    //    void WritePasswordChecked();
    void StartGetJour(); // start fet journal thread
    void StartReadFile();
    void StartSaveJour(int, QAbstractItemModel *, QString);

private slots:
    void TryGetJourByUSB();
    void GetJour();
    void JourFileChoosed(QString &file);
    void EraseJour();
    void SaveJour();
    //    void WritePasswordCheck(QString psw);
    void Done(QString msg, int);
    void Error(QString msg);

public slots:

private:
    QString JourFile;
    int JourType;
    bool ok;
    Journals *JourFuncs;
    QSortFilterProxyModel *ProxyWorkModel, *ProxySysModel, *ProxyMeasModel;
    Error::Msg WriteCheckPassword();
    void StartReadJourFile();
};

#endif // JOURNALDIALOG_H

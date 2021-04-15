#ifndef JOURNALDIALOG_H
#define JOURNALDIALOG_H

#include "../module/journals.h"
#include "../widgets/udialog.h"

#include <QMessageBox>
#include <QProgressDialog>
#include <QTableView>

#define MAXSWJNUM 262144

class JournalDialog : public UDialog
{
    Q_OBJECT
public:
    JournalDialog(UniquePointer<Journals> jour, QWidget *parent = nullptr);
    ~JournalDialog();

    quint8 start;
    int SaveI;

private:
    QMessageBox *MsgBox;
    QProgressDialog *progress;

    void SetupUI();
    QWidget *JourTab(DataTypes::FilesEnum jourtype);
    int GetJourNum(const QString &objname);
    //    void setConnections() override;

signals:
    void StartGetJour();
    void StartReadFile();
    void StartSaveJour(DataTypes::FilesEnum, QAbstractItemModel *, QString);

private slots:
    void JourFileChoosed(QString &file);
    void EraseJour();
    void SaveJour(DataTypes::FilesEnum jourType);
    void Done(QString msg, DataTypes::FilesEnum jourType);
    void Error(QString msg);

public slots:

private:
    QString JourFile;
    bool ok;
    UniquePointer<Journals> m_jour;
    QSortFilterProxyModel *ProxyWorkModel, *ProxySysModel, *ProxyMeasModel;
    void StartReadJourFile();
    QMap<DataTypes::FilesEnum, QTableView *> views;
};

#endif // JOURNALDIALOG_H

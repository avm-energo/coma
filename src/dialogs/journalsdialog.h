#ifndef JOURNALDIALOG_H
#define JOURNALDIALOG_H

<<<<<<< HEAD
#include "../module/basejournalmodel.h"
#include "../module/journals.h"
=======
#include "../journals/journals.h"
>>>>>>> develop
#include "../widgets/udialog.h"

#include <QMessageBox>
#include <QProgressDialog>
#include <QTableView>

//#define MAXSWJNUM 262144

class JournalDialog : public UDialog
{
    Q_OBJECT
public:
    JournalDialog(UniquePointer<Journals> jour, QWidget *parent = nullptr);
    ~JournalDialog();

signals:
    void StartSaveJour(DataTypes::FilesEnum, QString);

private:
    void SetupUI();
    QWidget *JourTab(DataTypes::FilesEnum jourtype);
    int GetJourNum(const QString &objname);

private slots:
    void JourFileChoosed(QString &file);
    void EraseJour();
    void SaveJour(DataTypes::FilesEnum jourType);
    void Done(QString msg);
    void Error(QString msg);

private:
    QString JourFile;
    UniquePointer<Journals> m_jour;
    QSortFilterProxyModel *ProxyWorkModel, *ProxySysModel, *ProxyMeasModel;
    QProgressDialog *progress;
    QMap<DataTypes::FilesEnum, QTableView *> views;
};

#endif // JOURNALDIALOG_H

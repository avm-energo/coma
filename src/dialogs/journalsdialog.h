#pragma once

//#include "../journals/basejournalmodel.h"
//#include "../journals/journals.h"
#include "../journals/basejournal.h"
#include "../widgets/udialog.h"

#include <QMessageBox>
#include <QProgressDialog>
#include <QSortFilterProxyModel>
#include <QTableView>

//#define MAXSWJNUM 262144

using Journal = journals::BaseJournal;

class JournalDialog : public UDialog
{
    Q_OBJECT
public:
    JournalDialog(const QList<Journal *> &journals, QWidget *parent = nullptr);
    ~JournalDialog();

signals:
    void startSaveJour(DataTypes::FilesEnum, QString);

private:
    void setupUI(const QList<Journal *> &journals);
    QWidget *jourTab(DataTypes::FilesEnum jourtype);
    int getJourNum(const QString &objname);

private slots:
    void jourFileChoosed(QString &file);
    void eraseJour();
    void saveJour(DataTypes::FilesEnum jourType);
    void done(QString msg);
    void error(QString msg);

private:
    QString JourFile;
    // QSortFilterProxyModel *proxyWorkModel, *proxySysModel, *proxyMeasModel;
    QProgressDialog *progress;
    QMap<DataTypes::FilesEnum, QTableView *> views;
};

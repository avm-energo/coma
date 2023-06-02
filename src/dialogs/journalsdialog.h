#pragma once

//#include "../journals/basejournalmodel.h"
//#include "../journals/journals.h"
#include "../journals/basejournal.h"
#include "../widgets/udialog.h"

#include <QMessageBox>
#include <QProgressDialog>
#include <QSortFilterProxyModel>
#include <QTableView>

using Journal = journals::BaseJournal;

class JournalDialog : public UDialog
{
    Q_OBJECT
public:
    JournalDialog(const ModuleSettings &settings, QWidget *parent = nullptr);

signals:
    void startSaveJour(QString);

private:
    void createJournals(const ModuleSettings &settings);
    void setupUI();
    QWidget *jourTab(const Journal *journal);
    // int getJourNum(const QString &objname);

private slots:
    void tabChanged(const int index);
    // void jourFileChoosed(QString &file);
    void eraseJour();
    void saveJour(DataTypes::FilesEnum jourType);
    void done(const QString &msg);
    void error(const QString &msg);

private:
    int currentTab;
    QList<Journal *> journals;
    // QString JourFile;
    QProgressDialog *progress;
    DataTypesProxy proxy;
};

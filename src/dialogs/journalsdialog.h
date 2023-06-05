#pragma once

#include "../widgets/udialog.h"
#include "journaltabwidget.h"

#include <map>

//#include "../journals/basejournalmodel.h"
//#include "../journals/journals.h"
//#include "../journals/basejournal.h"
//#include <QMessageBox>
//#include <QProgressDialog>
//#include <QSortFilterProxyModel>
//#include <QTableView>

class JournalDialog : public UDialog
{
    Q_OBJECT
public:
    JournalDialog(const ModuleSettings &settings, QWidget *parent = nullptr);

private:
    using JournalTab = journals::JournalTabWidget;
    using JournalType = journals::JournalType;
    DataTypesProxy proxy;
    std::map<JournalType, JournalTab *> journals;

    void createJournals(const ModuleSettings &settings);
    void setupUI();

private slots:
    void receivedJournalFile(const QVariant &jourData);

    // QWidget *jourTab(const Journal *journal);
    // int getJourNum(const QString &objname);

    // private slots:
    // void tabChanged(const int index);
    // void jourFileChoosed(QString &file);
    // void eraseJour();
    // void saveJour(DataTypes::FilesEnum jourType);
    // void done(const QString &msg);
    // void error(const QString &msg);

    // signals:
    //    void startSaveJour(QString);
    // private:
    // int currentTab;
    // QList<Journal *> journals;
    // QString JourFile;
    // QProgressDialog *progress;
};

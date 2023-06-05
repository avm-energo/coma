#pragma once

#include "../journals/basejournal.h"
#include "../s2/s2datatypes.h"
#include "../widgets/QProgressIndicator.h"

#include <QDialog>
#include <QProgressDialog>
#include <QWidget>

namespace journals
{

class JournalTabWidget : public QWidget
{
    Q_OBJECT
private:
    using Journal = journals::BaseJournal;
    Journal *journal;
    ETableView *modelView;
    QDialog *getProgressDialog;
    QProgressIndicator *getProgressIndicator;
    QProgressDialog *saveProgressDialog;
    S2DataTypes::S2BFile journalFile;

    void setupProgressDialogs();
    void setupUI();

private slots:
    void getJournal();
    void eraseJournal();
    void saveExcelJournal();
    void saveBinaryJournal();
    void done(const QString &message);
    void error(const QString &message);

public:
    JournalTabWidget(Journal *jour, QWidget *parent = nullptr);

public slots:
    void setJournalFile(const S2DataTypes::S2BFile &jourFile);
};

}

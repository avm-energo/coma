#pragma once

#include "../s2/s2datatypes.h"
#include "../widgets/QProgressIndicator.h"
#include "basejournal.h"

#include <QDialog>
#include <QProgressDialog>
#include <QWidget>

namespace journals
{

class JournalTabWidget : public QWidget
{
    Q_OBJECT
private:
    BaseJournal *journal;
    ETableView *modelView;
    QDialog *getProgressDialog;
    QProgressIndicator *getProgressIndicator;
    QProgressDialog *saveProgressDialog;
    S2DataTypes::S2BFile journalFile;

    void setupProgressDialogs();
    void setupUI();
    QString getSuggestedFilename();

private slots:
    void getJournal();
    void eraseJournal();
    void saveExcelJournal();
    void saveBinaryJournal();
    void done(const QString &message);
    void error(const QString &message);

public:
    JournalTabWidget(BaseJournal *jour, QWidget *parent = nullptr);
    void setJournalFile(const S2DataTypes::S2BFile &jourFile);
    const QString &getJournalName() const;
};

}

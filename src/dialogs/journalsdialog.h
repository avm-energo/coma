#ifndef JOURNALDIALOG_H
#define JOURNALDIALOG_H

#include "../journals/basejournalmodel.h"
#include "../journals/journals.h"
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
    void startSaveJour(DataTypes::FilesEnum, QString);

private:
    void setupUI();
    QWidget *jourTab(DataTypes::FilesEnum jourtype);
    int getJourNum(const QString &objname);
    void createProgressDialog();

private slots:
    void jourFileChoosed(QString &file);
    void eraseJour();
    void saveJour(DataTypes::FilesEnum jourType);
    void done(QString msg);
    void error(QString msg);

private:
    QString JourFile;
    UniquePointer<Journals> m_jour;
    QSortFilterProxyModel *proxyWorkModel, *proxySysModel, *proxyMeasModel;
    QProgressDialog *progress;
    QMap<DataTypes::FilesEnum, QTableView *> views;
};

#endif // JOURNALDIALOG_H

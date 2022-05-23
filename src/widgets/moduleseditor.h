#ifndef MODULESEDITOR_H
#define MODULESEDITOR_H

#include <QDir>
#include <QHBoxLayout>
#include <QStandardItemModel>
#include <QTableView>
#include <QTreeView>
#include <QWidget>

class ModulesEditor : public QWidget
{
    Q_OBJECT

private:
    enum WorkspaceType
    {
        Master = 1,
        Slave = 2
    };

    //QVBoxLayout *master;
    //QVBoxLayout *slave;
    QStandardItemModel *masterModel;
    QTableView *masterView;
    QStandardItemModel *slaveModel;
    QTreeView *slaveView;

    void SetupUI(QSize&);
    void SetupModelView();
    QVBoxLayout *GetWorkspace(WorkspaceType);

public:
    explicit ModulesEditor(QWidget *parent = nullptr);
    QDir UnpackData();
    void ReadModulesToModel();

signals:


public slots:
    void Close();

};

#endif // MODULESEDITOR_H

#ifndef MODULESEDITOR_H
#define MODULESEDITOR_H

#include <QDialog>
#include <QDir>
#include <QHBoxLayout>
#include <QStandardItemModel>
#include <QTableView>
#include <QTreeView>
#include <QWidget>
#include <QtXml>

class ModulesEditor : public QDialog
{
    Q_OBJECT

private:
    enum WorkspaceType
    {
        Master = 1,
        Slave = 2
    };

    QStandardItemModel *masterModel;
    QTableView *masterView;
    QStandardItemModel *slaveModel;
    QTreeView *slaveView;

    void SetupUI(QSize pSize);
    QVBoxLayout *GetWorkspace(WorkspaceType);
    QDir UnpackData();
    void ReadModulesToMasterModel();
    void ReadModuleToSlaveModel();
    QStandardItemModel *CreateMasterModel(const int rows, const int cols);
    QStandardItemModel *CreateSlaveModel();
    void ParseXmlToMasterModel(const QDomNode &node, const int &index);
    void ParseXmlToSlaveModel(QDomNode &node, int index, QStandardItem *parent);

public:
    explicit ModulesEditor(QWidget *parent = nullptr);

signals:

public slots:
    void Close();
    void MasterItemSelected(const QModelIndex &index);
};

#endif // MODULESEDITOR_H

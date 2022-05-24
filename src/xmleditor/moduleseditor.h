#ifndef MODULESEDITOR_H
#define MODULESEDITOR_H

#include <QDialog>
#include <QDir>
#include <QHBoxLayout>
#include <QStandardItemModel>
#include <QTableView>
#include <QTreeView>
#include <QtXml>
#include <QWidget>

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
    QStandardItemModel *CreateSlaveModel(const int&);
    void ReadModulesToMasterModel();
    void ReadModuleToSlaveModel();
    void ParseXmlToMasterModel(const QDomNode &node, const int &index, QStandardItemModel *model);
    void ParseXmlToSlaveModel(const QDomNode &node, QStandardItemModel *model);

public:
    explicit ModulesEditor(QWidget *parent = nullptr);

signals:

public slots:
    void Close();
    void MasterItemSelected(const QModelIndex &index);

};

#endif // MODULESEDITOR_H

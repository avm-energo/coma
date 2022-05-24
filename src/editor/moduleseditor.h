#ifndef MODULESEDITOR_H
#define MODULESEDITOR_H

#include <QDir>
#include <QHBoxLayout>
#include <QStandardItemModel>
#include <QTableView>
#include <QTreeView>
#include <QtXml>
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

    QStandardItemModel *masterModel;
    QTableView *masterView;
    QStandardItemModel *slaveModel;
    QTreeView *slaveView;

    void SetupUI(QSize&);
    QVBoxLayout *GetWorkspace(WorkspaceType);
    QDir UnpackData();
    void ReadModulesToModel();
    void SearchModule(const QDomNode &node, const int &index, QStandardItemModel *model);

public:
    explicit ModulesEditor(QWidget *parent = nullptr);

signals:


public slots:
    void Close();

};

#endif // MODULESEDITOR_H

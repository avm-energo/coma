#ifndef XMLEDITOR_H
#define XMLEDITOR_H

#include <QDialog>
#include <QDir>
#include <QHBoxLayout>
#include <QStandardItemModel>
#include <QTableView>
#include <QTreeView>
#include <QtXml>

class XmlEditor : public QDialog
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
    void ReadModulesToMasterModel();
    void ReadModuleToSlaveModel();
    QStandardItemModel *CreateMasterModel(const int rows, const int cols);
    QStandardItemModel *CreateSlaveModel();
    void ParseXmlToMasterModel(const QDomNode &node, const int &index);
    void ParseXmlToSlaveModel(QDomNode &node, int index, QStandardItem *parent);

public:
    explicit XmlEditor(QWidget *parent = nullptr);

signals:

public slots:
    void Close();
    void MasterItemSelected(const QModelIndex &index);
};

#endif // XMLEDITOR_H

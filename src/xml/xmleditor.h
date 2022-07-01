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
    QStandardItemModel *CreateMasterModel(const int rows);
    QStandardItemModel *CreateSlaveModel();

    /// Parsing section
    void ParseXmlToMasterModel(const QDomNode &node, const QString &filename, int &index);
    void ParseXmlToSlaveModel(QDomNode &node, int index, QStandardItem *parent);
    int ParseXmlFindAllAttributes(QDomNode &domElement, QStandardItem *element);

public:
    explicit XmlEditor(QWidget *parent = nullptr);

public slots:
    void Close();
    void MasterItemSelected(const QModelIndex &index);
};

#endif // XMLEDITOR_H

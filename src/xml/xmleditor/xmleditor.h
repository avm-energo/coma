#ifndef XMLEDITOR_H
#define XMLEDITOR_H

#include "models/mastermodel.h"

#include <QDialog>
#include <QHBoxLayout>
#include <QStackedWidget>
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

    // Master items
    MasterModel *masterModel;
    QTableView *masterView;

    // Slave items
    QStackedWidget *stackWidget;
    QTableView *tableSlaveView;

    void SetupUI(QSize pSize);
    QVBoxLayout *GetWorkspace(WorkspaceType);
    void CreateMasterModel();

public:
    explicit XmlEditor(QWidget *parent = nullptr);

public slots:
    void Close();
};

#endif // XMLEDITOR_H

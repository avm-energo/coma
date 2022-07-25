#ifndef XMLEDITOR_H
#define XMLEDITOR_H

#include "models/mastermodel.h"
#include "models/modelmanager.h"

#include <QDialog>
#include <QHBoxLayout>
#include <QTableView>
#include <QtXml>

class XmlEditor : public QDialog
{
    Q_OBJECT

private:
    // Master items
    MasterModel *masterModel;
    QTableView *masterView;

    // Slave items
    ModelManager *manager;
    QTableView *tableSlaveView;

    void SetupUI(QSize pSize);
    QVBoxLayout *GetMasterWorkspace();
    QVBoxLayout *GetSlaveWorkspace();
    void CreateMasterModel();

public:
    XmlEditor() = delete;
    explicit XmlEditor(QWidget *parent);

public slots:
    void Close();
    void CreateItem();
    void EditItem();
};

#endif // XMLEDITOR_H

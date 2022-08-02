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
    enum DialogType : quint16
    {
        Create = 0,
        Edit,
        Remove
    };

    // Master items
    MasterModel *masterModel;
    QTableView *masterView;

    // Slave items
    ModelManager *manager;
    QTableView *tableSlaveView;

    void SetupUI(QSize pSize);
    QVBoxLayout *GetMasterWorkspace();
    QVBoxLayout *GetSlaveWorkspace();
    void SlaveModelDialog(DialogType dlgType);

public:
    XmlEditor() = delete;
    explicit XmlEditor(QWidget *parent);

public slots:
    void Close();
    void CreateModule();
    void EditModule();
    void RemoveModule();
    void SaveModule();
};

#endif // XMLEDITOR_H

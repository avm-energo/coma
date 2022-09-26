#ifndef XMLEDITOR_H
#define XMLEDITOR_H

#include "datacontroller.h"
#include "models/mastermodel.h"
#include "models/modelmanager.h"

#include <QDialog>
#include <QHBoxLayout>
#include <QTableView>

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
    DataController *dc;
    // Master items
    MasterModel *masterModel;
    QTableView *masterView;
    // Slave items
    ModelManager *manager;
    QTableView *tableSlaveView;

    void setupUI(QSize pSize);
    QVBoxLayout *getMasterWorkspace();
    QVBoxLayout *getSlaveWorkspace();
    void actionDialog(DialogType dlgType, QTableView *srcView);
    void setFontBolding(int row, bool state);

public:
    XmlEditor() = delete;
    explicit XmlEditor(QWidget *parent);
    virtual void reject() override;

public slots:
    void savingAsk();
    void saveModule();
};

#endif // XMLEDITOR_H

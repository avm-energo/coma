#pragma once

#include <QDialog>
#include <QHBoxLayout>
#include <QTableView>

class DataController;
class MasterModel;
class ModelManager;

/// \brief Главный класс редактора XML.
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

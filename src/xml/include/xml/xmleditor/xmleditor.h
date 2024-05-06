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

    /// \brief Настройка интерфейса диалогового окна редактора.
    void setupUI(QSize pSize);
    /// \brief Возвращает рабочее пространство master (левая часть окна).
    QVBoxLayout *getMasterWorkspace();
    /// \brief Возвращает рабочее пространство slave (правая часть окна).
    QVBoxLayout *getSlaveWorkspace();
    /// \brief Слот для создания диалогового окна создания, редактирования или удаления выбранного элемента.
    void actionDialog(DialogType dlgType, QTableView *srcView);
    /// \brief Слот для смены bolding текста элемента, который был изменён (для master workspace).
    void setFontBolding(int row, bool state);

public:
    explicit XmlEditor(QWidget *parent = nullptr);
    /// \brief Действия, выполняемые при закрытии окна редактора.
    virtual void reject() override;

public slots:
    /// \brief Слот для создания диалогового окна о сохранении изменений в выбранном модуле.
    void savingAsk();
    /// \brief Слот для сохранения или сброса изменений настроек модуля.
    void saveModule();
};

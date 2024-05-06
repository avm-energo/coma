#pragma once

#include <QProgressDialog>
#include <dialogs/abstractinterfacedialog.h>

class InterfaceEthernetDialog final : public AbstractInterfaceDialog
{
    Q_OBJECT
public:
    explicit InterfaceEthernetDialog(QWidget *parent = nullptr);
    ~InterfaceEthernetDialog() noexcept;

    void setupUI() override;
    void setInterface(QModelIndex index) override;

private:
    void addInterface() override;
    void scanInterface();
    void acceptedInterface() override;
    bool updateModel() override;

    QList<quint32> m_hosts;

    /// Кандидаты для отдельного класса
    QProgressDialog *m_progress;
    void handlePing();
    void handlePingFinish();
    void handlePortFinish();
    void createPingTask(quint32 ip);
    void createPortTask();
    ///

signals:
    void pingFinished();
    void modelUpdated();
};

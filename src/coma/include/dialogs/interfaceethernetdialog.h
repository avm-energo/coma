#pragma once

#include <dialogs/abstractinterfacedialog.h>

#include <QProgressDialog>
#include <QVBoxLayout>

class InterfaceEthernetDialog final : public AbstractInterfaceDialog
{
    Q_OBJECT

    QList<quint32> m_hosts;

    // ### UI ###
    QHBoxLayout *m_mainLayout = new QHBoxLayout;
    QVBoxLayout *m_tableButtonLayout = new QVBoxLayout;
    QVBoxLayout *m_addScanLayout = new QVBoxLayout;

    QWidget *m_addWidget = new QWidget(this);
    QWidget *m_scanWidget = new QWidget(this);
    // ##########

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
    void deleteInterface();

    void setupAddWidget();

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
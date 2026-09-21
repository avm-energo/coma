#pragma once

#include <dialogs/connDialogs/abstractinterfacedialog.h>

#include <QList>
#include <QVBoxLayout>

class QGroupBox;

class InterfaceEthernetDialog final : public AbstractInterfaceDialog
{
    Q_OBJECT

    // ### UI ###
    QHBoxLayout *m_mainLayout = new QHBoxLayout;
    QVBoxLayout *m_tableButtonLayout = new QVBoxLayout;
    QVBoxLayout *m_addScanLayout = new QVBoxLayout;

    QGroupBox *m_addWidget;
    // ##########

public:
    explicit InterfaceEthernetDialog(QWidget *parent = nullptr);
    ~InterfaceEthernetDialog() noexcept;

    void setupUI() override;
    void setInterface(QModelIndex index) override;

    /// \brief Отображает в таблице подключений найденные сканером устройства
    /// (имя не сохраняется, порт и адрес БС проставляются по умолчанию).
    void displayScanResults(const QList<quint32> &hosts);

private:
    void addInterface() override;
    void acceptedInterface() override;
    bool updateModel() override;
    void deleteInterface();
    void editInterface();

    void setupAddWidget();
    void rowSelected(const QModelIndex &current);
};
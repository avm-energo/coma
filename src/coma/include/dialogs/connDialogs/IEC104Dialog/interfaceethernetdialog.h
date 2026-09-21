#pragma once

#include <dialogs/connDialogs/abstractinterfacedialog.h>

#include <QList>
#include <QVBoxLayout>

class QGroupBox;

class InterfaceEthernetDialog : public AbstractInterfaceDialog
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
    /// (имя не сохраняется, порт и адрес проставляются по умолчанию).
    void displayScanResults(const QList<quint32> &hosts);

    /// \brief Порт по умолчанию (используется и формой добавления, и сканером портов).
    virtual quint16 defaultPort() const;
    /// \brief Адрес (БС для IEC-104 / устройства для Modbus TCP) по умолчанию.
    virtual quint16 defaultAddress() const;

protected:
    /// \brief Группа реестра настроек, в которой хранятся сохранённые подключения.
    virtual QString settingsGroup() const;
    /// \brief Подпись поля адреса (без двоеточия) — используется и как заголовок формы, и как
    /// заголовок колонки таблицы.
    virtual QString addressLabel() const;
    /// \brief Ключ в реестре настроек, под которым сохраняется значение поля адреса.
    virtual QString addressSettingsKey() const;
    /// \brief Текст ошибки, показываемый при попытке сохранить нулевой адрес.
    virtual QString zeroAddressError() const;
    /// \brief Собирает конкретный объект настроек соединения по данным строки таблицы/формы.
    /// \return `nullptr`, если данные невалидны (владение при успехе переходит вызывающему).
    virtual BaseSettings *buildSettings(const QString &ip, quint16 port, quint16 address) const;
    /// \brief Оборачивает уже собранный (см. buildSettings()) объект настроек в ConnectionSettings —
    /// отдельный хук нужен, поскольку `ConnectionSettings::settings` — это `std::variant` по
    /// конкретным типам настроек, и общего пути через указатель на базовый класс тут нет.
    virtual ConnectionSettings wrapSettings(const QString &name, BaseSettings *settings) const;

private:
    void addInterface() override;
    void acceptedInterface() override;
    bool updateModel() override;
    void deleteInterface();
    void editInterface();

    void setupAddWidget();
    void rowSelected(const QModelIndex &current);
};
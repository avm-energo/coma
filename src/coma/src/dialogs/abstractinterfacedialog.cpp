#include "dialogs/abstractinterfacedialog.h"

#include <QLayout>
#include <QPushButton>

AbstractInterfaceDialog::AbstractInterfaceDialog(QWidget *parent) : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

void AbstractInterfaceDialog::setupUI()
{
    QPushButton *pb = new QPushButton("Отмена");
    connect(pb, &QAbstractButton::clicked, this, &QDialog::close);
    QVBoxLayout *lyout = static_cast<QVBoxLayout *>(layout());
    lyout->addWidget(pb);
}

bool AbstractInterfaceDialog::checkSize()
{
    auto model = m_tableView->model();
    if (model != nullptr)
        return (model->rowCount() == MAXREGISTRYINTERFACECOUNT);
    return false;
}

void AbstractInterfaceDialog::apply(BaseSettings &connection)
{
    connection.m_silentInterval = UserSettings::get(UserSettings::SettingName::SilentInterval);
    connection.m_maxErrors = UserSettings::get(UserSettings::SettingName::ErrorCount);
    connection.m_maxTimeouts = UserSettings::get(UserSettings::SettingName::TimeoutCount);
    connection.m_isLoggingEnabled = UserSettings::get(UserSettings::SettingName::LoggingEnabled);
    connection.m_logLevel = QString(UserSettings::get(UserSettings::SettingName::LogLevel));
}

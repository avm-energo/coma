#include "abstractinterfacedialog.h"

#include <QLayout>
#include <QPushButton>

AbstractInterfaceDialog::AbstractInterfaceDialog(QWidget *parent)
    : QDialog(parent), m_settings(Settings::UserSettings::GetInstance())
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

void AbstractInterfaceDialog::fill(BaseSettings &connection)
{
    connection.m_silentInterval = m_settings.get<Settings::SilentInterval>();
    connection.m_maxErrors = m_settings.get<Settings::ErrorCount>();
    connection.m_maxTimeouts = m_settings.get<Settings::TimeoutCount>();
    connection.m_isLoggingEnabled = m_settings.get<Settings::LoggingEnabled>();
}

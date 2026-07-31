#include "dialogs/abstractinterfacedialog.h"

#include <libavm-gen/settings.h>
#include <libavm-widgets/pbfunc.h>

#include <QLayout>
#include <QPushButton>

AbstractInterfaceDialog::AbstractInterfaceDialog(QWidget *parent) : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

void AbstractInterfaceDialog::setupUI()
{
    QPushButton *pb = PBFunc::New(this, "", "Назад", this, &QDialog::close);
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

void AbstractInterfaceDialog::apply(BaseSettings *connection)
{
    connection->set("silentInterval", Settings::get("silentInterval", 10000));
    connection->set("maxErrors", Settings::get("errorCount", 5));
    connection->set("maxTimeouts", Settings::get("timeoutCount", 5));
    connection->set("isLoggingEnabled", Settings::get("loggingEnabled", true));
    connection->set("logLevel", QString(Settings::get("logLevel", "Warn")));
}

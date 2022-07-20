#include "xmleditdialog.h"

#include <QMessageBox>

XmlEditDialog::XmlEditDialog(QWidget *parent) : QDialog(parent, Qt::Window), changes(false)
{
    this->exec();
}

void XmlEditDialog::reject()
{
    if (changes)
    {
        auto resBtn = QMessageBox::question(this, "Saving", tr("Close window?\n"),
            QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
        if (resBtn == QMessageBox::Yes)
            QDialog::reject();
    }
}

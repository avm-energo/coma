#include "emessagebox.h"

#include <QApplication>
#include <QMessageBox>
namespace EMessageBox
{

void information(QWidget *parent, const QString &title, const QString &text, const QString &detailedText)
{
    QMessageBox::information(parent, title, text, QMessageBox::Ok);
}

void error(QWidget *parent, const QString &title, const QString &text, const QString &detailedText)
{
    QMessageBox::critical(parent, title, text, QMessageBox::Ok);
}

}

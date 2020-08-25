#ifndef EMESSAGEBOX_H
#define EMESSAGEBOX_H

#include <QWidget>
namespace EMessageBox
{

void information(QWidget *parent, const QString &title, const QString &text, const QString &detailedText = QString());

void error(QWidget *parent, const QString &title, const QString &text, const QString &detailedText = QString());

}

#endif // EMESSAGEBOX_H

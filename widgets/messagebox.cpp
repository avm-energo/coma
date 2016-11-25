#include <QApplication>
#include <QPushButton>
#include <QMessageBox>
#include "messagebox.h"

#if QT_VERSION >= 0x040600
#include <QScopedPointer>
#else
#include <QSharedPointer>
#endif


/*
    Copyright (c) 2009-10 Qtrac Ltd. All rights reserved.

    This module is free software: you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version. It is provided
    for educational purposes and is distributed in the hope that it will
    be useful, but WITHOUT ANY WARRANTY; without even the implied
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
    the GNU General Public License for more details.
*/

namespace MessageBox2
{

void information(QWidget *parent, const QString &title,
                 const QString &text, const QString &detailedText)
{
#if QT_VERSION >= 0x040600
    QScopedPointer<QMessageBox> messageBox(new QMessageBox(parent));
#else
    QSharedPointer<QMessageBox> messageBox(new QMessageBox(parent));
#endif
    if (parent)
        messageBox->setWindowModality(Qt::WindowModal);
    messageBox->setWindowTitle(QString("%1 - %2").arg(QApplication::applicationName()).arg(title));
    messageBox->setText(text);
    if (!detailedText.isEmpty())
        messageBox->setInformativeText(detailedText);
    messageBox->setIcon(QMessageBox::Information);
    messageBox->addButton(QMessageBox::Ok);
    messageBox->exec();
}

void error(QWidget *parent, const QString &title,
                 const QString &text, const QString &detailedText)
{
#if QT_VERSION >= 0x040600
    QScopedPointer<QMessageBox> messageBox(new QMessageBox(parent));
#else
    QSharedPointer<QMessageBox> messageBox(new QMessageBox(parent));
#endif
    if (parent)
        messageBox->setWindowModality(Qt::WindowModal);
    messageBox->setWindowTitle(QString("%1 - %2").arg(QApplication::applicationName()).arg(title));
    messageBox->setText(text);
    if (!detailedText.isEmpty())
        messageBox->setInformativeText(detailedText);
    messageBox->setIcon(QMessageBox::Critical);
    messageBox->addButton(QMessageBox::Ok);
    messageBox->exec();
}

bool question(QWidget *parent, const QString &title,
              const QString &text, const QString &detailedText,
              const QString &yesText, const QString &noText)
{
#if QT_VERSION >= 0x040600
    QScopedPointer<QMessageBox> messageBox(new QMessageBox(parent));
#else
    QSharedPointer<QMessageBox> messageBox(new QMessageBox(parent));
#endif
    if (parent)
        messageBox->setWindowModality(Qt::WindowModal);
    messageBox->setWindowTitle(QString("%1 - %2").arg(QApplication::applicationName()).arg(title));
    messageBox->setText(text);
    if (!detailedText.isEmpty())
        messageBox->setInformativeText(detailedText);
    messageBox->setIcon(QMessageBox::Question);
    QPushButton *yesButton = messageBox->addButton(yesText, QMessageBox::AcceptRole);
    yesButton->setIcon(QIcon(":/res/ok.png"));
    QPushButton *noButton = messageBox->addButton(noText, QMessageBox::RejectRole);
    noButton->setIcon(QIcon(":/res/cross.png"));
    messageBox->setDefaultButton(yesButton);
    messageBox->exec();
    QPushButton *pb = qobject_cast<QPushButton *>(messageBox->clickedButton());
    return (pb == yesButton);
}

}

#include "dialogs/errordialog.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <gen/colors.h>
#include <gen/error.h>
#include <widgets/errorprotocolwidget.h>

ErrorDialog::ErrorDialog(QWidget *parent) : QDialog(parent)
{
    SetupUI();
}

void ErrorDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    auto ErrorWidget = new ErrorProtocolWidget;
    ErrorWidget->setObjectName("errorwidget");

    ErrorWidget->setAutoFillBackground(true);
    lyout->addWidget(ErrorWidget);
    QPushButton *pb = new QPushButton("Закрыть");
    connect(pb, &QAbstractButton::clicked, this, &QWidget::close);
    hlyout->addStretch(100);
    hlyout->addWidget(pb);
    hlyout->addStretch(100);
    lyout->addLayout(hlyout);
    setLayout(lyout);
    setAttribute(Qt::WA_DeleteOnClose);
}

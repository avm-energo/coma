#include "errordialog.h"

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../widgets/errorprotocolwidget.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
ErrorDialog::ErrorDialog(QWidget *parent) : QDialog(parent)
{
    SetupUI();
}

void ErrorDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    ErrorWidget = new ErrorProtocolWidget;
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

void ErrorDialog::AddErrMsg(ErrorMsg msg)
{
    ErrorWidget->AddRowToProt(msg);
}

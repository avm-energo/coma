#include "errordialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>

ErrorDialog::ErrorDialog(QWidget *parent) :
    QDialog(parent)
{
    SetupUI();
}

void ErrorDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    ErrorWidget = new ErrorProtocolWidget(this);
    ErrorWidget->setObjectName("errorwidget");
    QString ErrWss = "QWidget {background-color: "+QString(ERPROTCLR)+";}";
    ErrorWidget->setStyleSheet(ErrWss);
    ErrorWidget->setAutoFillBackground(true);
    lyout->addWidget(ErrorWidget);
    QPushButton *pb = new QPushButton("Закрыть");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(close()));
    hlyout->addStretch(100);
    hlyout->addWidget(pb);
    hlyout->addStretch(100);
    lyout->addLayout(hlyout);
    setLayout(lyout);
    setAttribute(Qt::WA_DeleteOnClose);
}

void ErrorDialog::AddErrMsg(publicclass::ermsg msg)
{
    ErrorWidget->AddRowToProt(msg);
}
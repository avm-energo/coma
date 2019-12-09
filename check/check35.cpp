// check35.cpp
#include <QGridLayout>
#include <QGroupBox>
#include "check35.h"
#include "../gen/colors.h"
#include "../widgets/wd_func.h"

Check35::Check35() : QDialog()
{


}

Check35::~Check35()
{


}

QWidget *Check35::Bd1W(QWidget *parent)
{
    QGridLayout *glyout = new QGridLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QWidget *w = new QWidget(parent);
    int row = 0;

    QPushButton *Button = new QPushButton("Подать команду на реле №1");
    glyout->addWidget(Button, row,1,1,1,Qt::AlignTop);
    connect(Button, SIGNAL(clicked()), this, SLOT(Start_Timer()));

    row++;
    Button = new QPushButton("Подать команду на реле №2");
    glyout->addWidget(Button, row,1,1,1,Qt::AlignTop);
    connect(Button, SIGNAL(clicked()), this, SLOT(Start_Timer()));

    row++;
    Button = new QPushButton("Подать команду на реле №3");
    glyout->addWidget(Button, row,1,1,1,Qt::AlignTop);
    connect(Button, SIGNAL(clicked()), this, SLOT(Start_Timer()));

    row++;
    Button = new QPushButton("Подать команду на реле №4");
    glyout->addWidget(Button, row,1,1,1,Qt::AlignTop);
    connect(Button, SIGNAL(clicked()), this, SLOT(Start_Timer()));

    vlyout->addLayout(glyout);
    w->setLayout(vlyout);
    QString tmps = "QWidget {background-color: "+QString(UCONFCLR)+";}";
    w->setStyleSheet(tmps);
    return w;
}

QWidget *Check35::BdUI(int bdnum)
{
    Q_UNUSED(bdnum);
    return nullptr;
}

QWidget *Check35::CustomTab()
{
    QWidget *w = new QWidget;

    return w;
}

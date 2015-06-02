#include "oscdialog.h"
#include "canal.h"
#include "widgets/s_tqtableview.h"
#include "widgets/s_tablemodel.h"

#include <QVBoxLayout>
#include <QPushButton>

oscdialog::oscdialog(QWidget *parent) :
    QDialog(parent)
{

}

void oscdialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    s_tqTableView *tv = new s_tqTableView;
    tv->setObjectName("osctv");
    s_tablemodel *tm = new s_tablemodel;
    tv->setModel(tm);
    QPushButton *pb = new QPushButton("Получить данные по осциллограммам в памяти модуля");
    connect(pb,SIGNAL(clicked()),this,SLOT(GetOscInfo()));
    lyout->addWidget(pb);
    lyout->addWidget(tv);
    lyout->addStretch(90);
    OscInfo = new QByteArray;
}

void oscdialog::GetOscInfo()
{

}

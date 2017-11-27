#include <QVBoxLayout>
#include "../widgets/s_tqtableview.h"
#include "switchjournal.h"

SwitchJournal::SwitchJournal(QWidget *parent) : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

void SwitchJournal::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    s_tqTableView *tv = new s_tqTableView;
    tv->setObjectName("osctv");
    TableModel = new ETableModel;
    QPushButton *pb = new QPushButton("Получить журнал переключений");
    connect(pb,SIGNAL(clicked()),this,SLOT(LoadJournals()));
    if (pc.Emul)
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    pb = new QPushButton("Стереть журнал переключений");
    connect(pb,SIGNAL(clicked()),this,SLOT(EraseJournals()));
    if (pc.Emul)
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    lyout->addWidget(tv, 89);
    setLayout(lyout);
}

void SwitchJournal::LoadJournals()
{

}

void SwitchJournal::ShowJournal(QModelIndex idx)
{

}

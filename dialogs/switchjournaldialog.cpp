#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include "../widgets/messagebox.h"
#include "../gen/commands.h"
#include "../gen/eoscillogram.h"
#include "../widgets/getoscpbdelegate.h"
#include "../widgets/wd_func.h"
#include "switchjournaldialog.h"

SwitchJournalDialog::SwitchJournalDialog(QWidget *parent) : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

void SwitchJournalDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
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
    SwjTableView = new s_tqTableView;
    TableModel = new ETableModel;
    SwjTableView->setModel(TableModel);
    SwjTableView->horizontalHeader()->setVisible(false);
    SwjTableView->verticalHeader()->setVisible(false);
    lyout->addWidget(SwjTableView, 89);
    setLayout(lyout);
}

void SwitchJournalDialog::ProcessSWJournal(QByteArray &ba)
{
    SWJournalRecordStruct tmpswj;
    int SWJRecordSize = sizeof(SWJournalRecordStruct);
    int BaSize = ba.size();
    int BaPos = 0;
    int CurRow = 1;
    while ((BaPos + SWJRecordSize) < BaSize)
    {
        memcpy(&tmpswj, &(ba.data()[BaPos]), SWJRecordSize);
        SWJMap[tmpswj.Num] = tmpswj;
        TableModel->addRow();
        TableModel->setData(TableModel->index(CurRow, 0, QModelIndex()), QVariant(tmpswj.Num), Qt::EditRole);
        TableModel->setData(TableModel->index(CurRow, 1, QModelIndex()), QVariant(pc.UnixTime64ToString(tmpswj.Time)), Qt::EditRole);
        QStringList tmpsl = QStringList() << "D" << "G" << "CB";
        QString tmps = (tmpswj.TypeA < tmpsl.size()) ? tmpsl.at(tmpswj.TypeA) : "N/A";
        TableModel->setData(TableModel->index(CurRow, 2, QModelIndex()), QVariant(tmps), Qt::EditRole);
        TableModel->setData(TableModel->index(CurRow, 3, QModelIndex()), QVariant(QString::number(tmpswj.NumA)), Qt::EditRole);
        tmps = (tmpswj.Options & 0x00000001) ? "ВКЛ" : "ОТКЛ";
        TableModel->setData(TableModel->index(CurRow, 4, QModelIndex()), QVariant(tmps), Qt::EditRole);
        if (OscNums.contains(tmpswj.OscTime))
            tmps = "images/oscillogramm.png";
        else
            tmps = "images/hr.png";
        TableModel->setData(TableModel->index(CurRow, 5, QModelIndex()), QVariant(QIcon(tmps)), Qt::DecorationRole);
        TableModel->setData(TableModel->index(CurRow, 6, QModelIndex()), QVariant("Далее"), Qt::EditRole);
    }
    GetOscPBDelegate *dg = new GetOscPBDelegate;
    connect(dg,SIGNAL(clicked(QModelIndex)),this,SLOT(ShowJournal(QModelIndex)));
    tv->setItemDelegateForColumn(6,dg); // устанавливаем делегата (кнопки "Скачать") для соотв. столбца
    tv->resizeRowsToContents();
    tv->resizeColumnsToContents();
}

void SwitchJournalDialog::ProcessOscillograms()
{
    QByteArray OscInfo;
    quint32 OscInfoSize; // размер считанного буфера с информацией об осциллограммах
    quint32 RecordSize = sizeof(EOscillogram::GBoStruct); // GBo struct size
    OscInfoSize = MAXOSCBUFSIZE;
    OscInfo.resize(OscInfoSize);
    if ((Commands::GetBt(TECH_Bo, &(OscInfo.data()[0]), OscInfoSize)) != NOERROR)
    {
        WARNMSG("Ошибка при приёме буфера осциллограмм");
        return;
    }
    for (quint32 i = 0; i < OscInfoSize; i+= RecordSize)
    {
        EOscillogram::GBoStruct gbos;
        memcpy(&gbos, &(OscInfo.data()[i]), RecordSize);
        OscNums.push_back(gbos.UnixTime);
    }
}

void SwitchJournalDialog::LoadJournals()
{
    TableModel->addColumn("#");
    TableModel->addColumn("Time");
    TableModel->addColumn("TypeA");
    TableModel->addColumn("NumA");
    TableModel->addColumn("Options");
    TableModel->addColumn("Osc");
    TableModel->addColumn("Next");
    TableModel->addRow();
    TableModel->setData(TableModel->index(0, 0, QModelIndex()), QVariant("#"), Qt::EditRole);
    TableModel->setData(TableModel->index(0, 1, QModelIndex()), QVariant("Дата, время"), Qt::EditRole);
    TableModel->setData(TableModel->index(0, 2, QModelIndex()), QVariant("Аппарат"), Qt::EditRole);
    TableModel->setData(TableModel->index(0, 4, QModelIndex()), QVariant("Переключение"), Qt::EditRole);
    SwjTableView->setSpan(0, 2, 1, 2); // объединение 2 и 3 столбцов в 0 ряду
    SwjTableView->setSpan(0, 4, 1, 2); // объединение 4 и 5 столбцов в 0 ряду
    QByteArray SWJournal;
    quint32 SWJSize = sizeof(SWJournalRecordStruct) * MAXSWJNUM;
    SWJournal.resize(SWJSize);
    Commands::GetBt(TECH_SWJ, &(SWJournal.data()[0]), SWJSize); // в SWJSize - реальная длина в байтах
    ProcessOscillograms();
    ProcessSWJournal(SWJournal);
}

void SwitchJournalDialog::ShowJournal(QModelIndex idx)
{
    quint32 SWJNum = TableModel->data(idx.sibling(idx.row(),0),Qt::DisplayRole).toInt(&ok); // номер осциллограммы    if (!ok)
    {
        WARNMSG("");
        return;
    }
    SWJournalRecordStruct tmpswj = SWJMap[SWJNum];
    QDialog *dlg = new QDialog;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    glyout->addWidget(WDFunc::NewLBL(this, "Номер"), 0,0,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Дата, время"),0,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Аппарат"),0,2,1,2);
    glyout->addWidget(WDFunc::NewLBL(this, "Переключение"),0,4,1,2);
    glyout->addWidget(WDFunc::NewLBLT(this, QString::number(tmpswj.Num)), 1,0,1,1);
    glyout->addWidget(WDFunc::NewLBLT(this, pc.UnixTime64ToString(tmpswj.Time)),1,1,1,1);
    QStringList tmpsl = QStringList() << "D" << "G" << "CB";
    QString tmps = (tmpswj.TypeA < tmpsl.size()) ? tmpsl.at(tmpswj.TypeA) : "N/A";
    glyout->addWidget(WDFunc::NewLBLT(this, tmps),1,2,1,1);
    glyout->addWidget(WDFunc::NewLBLT(this, QString::number(tmpswj.NumA)),1,3,1,1);
    tmps = (tmpswj.Options & 0x00000001) ? "ВКЛ" : "ОТКЛ";
    glyout->addWidget(WDFunc::NewLBLT(this, tmps),1,4,1,2);
    glyout->addWidget(WDFunc::NewLBL(this, "Результат переключения"),2,0,1,6);
    glyout->addWidget(WDFunc::NewLBL(this, "Тип коммутации и коммутируемые фазы"),3,0,1,4);
    glyout->addWidget(WDFunc::NewLBLT(this, QString::number(tmpswj.OpResult)),3,4,1,1);
    if (OscNums.contains(tmpswj.OscTime))
    {
        QPushButton *pb = new QPushButton;
        pb->setIcon(QIcon("images/oscillogramm.png"));
        connect(pb,SIGNAL(clicked(bool)),this,SLOT(ShowOsc()));
        glyout->addWidget(pb,3,5,1,1);
    }
    else
        glyout->addWidget(WDFunc::NewLBL(this, "", "", "", QPixmap("images/hr.png")),3,5,1,1);
    vlyout->addLayout(glyout);

    dlg->setLayout(vlyout);
    dlg->exec();
}

void SwitchJournalDialog::EraseJournals()
{
    if (Commands::EraseTechBlock(TECH_SWJ) == NOERROR)
        MessageBox2::information(this, "Внимание", "Стёрто успешно");
    else
        MessageBox2::information(this, "Внимание", "Ошибка при стирании");
}

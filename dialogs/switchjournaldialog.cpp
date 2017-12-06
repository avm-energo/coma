#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include "../widgets/messagebox.h"
#include "../gen/commands.h"
#include "../widgets/getoscpbdelegate.h"
#include "../widgets/wd_func.h"
#include "switchjournaldialog.h"
#include "trendviewdialog.h"

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
    SWJMap.clear();
    while ((BaPos + SWJRecordSize) < BaSize)
    {
        memcpy(&tmpswj, &(ba.data()[BaPos]), SWJRecordSize);
        if ((tmpswj.Num != 0) && (!SWJMap.keys().contains(tmpswj.Num))) // пропуск пустых записей
        {
            SWJMap[tmpswj.Num] = tmpswj;
            TableModel->addRow();
            TableModel->setData(TableModel->index(CurRow, 0, QModelIndex()), QVariant(tmpswj.Num), Qt::EditRole);
            TableModel->setData(TableModel->index(CurRow, 1, QModelIndex()), QVariant(pc.UnixTime64ToString(tmpswj.Time)), Qt::EditRole);
            QStringList tmpsl = QStringList() << "D" << "G" << "CB";
            QString tmps = (tmpswj.TypeA < tmpsl.size()) ? tmpsl.at(tmpswj.TypeA) : "N/A";
            tmps += QString::number(tmpswj.NumA);
            TableModel->setData(TableModel->index(CurRow, 2, QModelIndex()), QVariant(tmps), Qt::EditRole);
            tmps = (tmpswj.Options & 0x00000001) ? "ВКЛ" : "ОТКЛ";
            TableModel->setData(TableModel->index(CurRow, 3, QModelIndex()), QVariant(tmps), Qt::EditRole);
            if (OscMap.keys().contains(tmpswj.OscTime))
                tmps = "images/oscillogramm.png";
            else
                tmps = "images/hr.png";
            TableModel->setData(TableModel->index(CurRow, 4, QModelIndex()), QVariant(QIcon(tmps)), Qt::DecorationRole);
            TableModel->setData(TableModel->index(CurRow, 5, QModelIndex()), QVariant("Далее"), Qt::EditRole);
        }
        BaPos += SWJRecordSize;
    }
    GetOscPBDelegate *dg = new GetOscPBDelegate(this);
    connect(dg,SIGNAL(clicked(QModelIndex)),this,SLOT(ShowJournal(QModelIndex)));
    SwjTableView->setItemDelegateForColumn(5,dg); // устанавливаем делегата (кнопки "Скачать") для соотв. столбца
    SwjTableView->resizeRowsToContents();
    SwjTableView->resizeColumnsToContents();
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
        OscMap[gbos.UnixTime] = gbos;
    }
}

void SwitchJournalDialog::LoadJournals()
{
    TableModel->ClearModel();
    TableModel->addColumn("#");
    TableModel->addColumn("Time");
    TableModel->addColumn("TypeANumA");
    TableModel->addColumn("Options");
    TableModel->addColumn("Osc");
    TableModel->addColumn("Next");
    TableModel->addRow();
    TableModel->SetRowTextAlignment(0, Qt::AlignVCenter | Qt::AlignCenter);
    TableModel->setData(TableModel->index(0, 0, QModelIndex()), QVariant("#"), Qt::EditRole);
    TableModel->setData(TableModel->index(0, 1, QModelIndex()), QVariant("Дата, время"), Qt::EditRole);
    TableModel->setData(TableModel->index(0, 2, QModelIndex()), QVariant("Аппарат"), Qt::EditRole);
    TableModel->setData(TableModel->index(0, 3, QModelIndex()), QVariant("Переключение"), Qt::EditRole);
    TableModel->setData(TableModel->index(0, 4, QModelIndex()), QVariant("Осц"), Qt::EditRole);
//    SwjTableView->setSpan(0, 3, 1, 2); // объединение 3 и 4 столбцов в 0 ряду
    QByteArray SWJournal;
    quint32 SWJSize = sizeof(SWJournalRecordStruct) * MAXSWJNUM;
    SWJournal.resize(SWJSize);
    Commands::GetBt(TECH_SWJ, &(SWJournal.data()[0]), SWJSize); // в SWJSize - реальная длина в байтах
    SWJournal.resize(SWJSize);
    ProcessOscillograms();
    ProcessSWJournal(SWJournal);
}

void SwitchJournalDialog::ShowJournal(QModelIndex idx)
{
    bool ok;
    quint32 SWJNum = TableModel->data(idx.sibling(idx.row(),0),Qt::DisplayRole).toInt(&ok); // номер осциллограммы
    if (!ok)
    {
        WARNMSG("");
        return;
    }
    SWJRecord = SWJMap[SWJNum];
    QDialog *dlg = new QDialog;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    glyout->addWidget(WDFunc::NewLBL(this, "Номер"), 0,0,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Дата, время"),0,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Аппарат"),0,2,1,2);
    glyout->addWidget(WDFunc::NewLBL(this, "Переключение"),0,4,1,2);
    glyout->addWidget(WDFunc::NewLBLT(this, QString::number(SWJRecord.Num)), 1,0,1,1);
    glyout->addWidget(WDFunc::NewLBLT(this, pc.UnixTime64ToString(SWJRecord.Time)),1,1,1,1);
    QStringList tmpsl = QStringList() << "D" << "G" << "CB";
    QString tmps = (SWJRecord.TypeA < tmpsl.size()) ? tmpsl.at(SWJRecord.TypeA) : "N/A";
    glyout->addWidget(WDFunc::NewLBLT(this, tmps),1,2,1,1);
    glyout->addWidget(WDFunc::NewLBLT(this, QString::number(SWJRecord.NumA)),1,3,1,1);
    tmps = (SWJRecord.Options & 0x00000001) ? "ВКЛ" : "ОТКЛ";
    glyout->addWidget(WDFunc::NewLBLT(this, tmps),1,4,1,2);
    glyout->addWidget(WDFunc::NewLBL(this, "Результат переключения"),2,0,1,6);
    glyout->addWidget(WDFunc::NewLBL(this, "Тип коммутации и коммутируемые фазы"),3,0,1,4);
    glyout->addWidget(WDFunc::NewLBLT(this, QString::number(SWJRecord.OpResult)),3,4,1,1);
    if (OscMap.keys().contains(SWJRecord.OscTime))
    {
        QPushButton *pb = new QPushButton;
        pb->setIcon(QIcon("images/oscillogramm.png"));
        connect(pb,SIGNAL(clicked(bool)),this,SLOT(ShowOsc()));
        glyout->addWidget(pb,3,5,1,1);
    }
    else
    {
        QPixmap *pm = new QPixmap("images/hr.png");
        glyout->addWidget(WDFunc::NewLBL(this, "", "", "", pm),3,5,1,1);
    }
    vlyout->addLayout(glyout);
    vlyout->addStretch(10);
    glyout = new QGridLayout;
    QStringList sl = QStringList() << "Значение тока при коммутации, А" << "Значение напряжения при коммутации, кВ" << \
                                      "Собственное время коммутации, мс" << "Полное время коммутации, мс" << \
                                      "Время перемещения главного контакта, мс" << "Время горения дуги, мс" << \
                                      "Время безоперационного простоя к моменту коммутации, ч" << \
                                      "Погрешность синхронной коммутации, мс";
    glyout->addWidget(WDFunc::NewLBL(this, "Измеренное значение"),0,0,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "A"),0,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "B"),0,2,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "C"),0,3,1,1);
    glyout->setColumnStretch(0, 10);
    int row = 1;
    glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
    for (int i=0; i<3; ++i)
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(SWJRecord.I[i], 'f', 1)),row,i+1,1,1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
    for (int i=0; i<3; ++i)
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(SWJRecord.U[i], 'f', 1)),row,i+1,1,1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
    for (int i=0; i<3; ++i)
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(SWJRecord.OwnTime[i], 'f', 1)),row,i+1,1,1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
    for (int i=0; i<3; ++i)
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(SWJRecord.FullTime[i], 'f', 1)),row,i+1,1,1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
    for (int i=0; i<3; ++i)
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(SWJRecord.MovTime[i], 'f', 1)),row,i+1,1,1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
    for (int i=0; i<3; ++i)
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(SWJRecord.ArchTime[i], 'f', 1)),row,i+1,1,1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
    for (int i=0; i<3; ++i)
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(SWJRecord.IdleTime[i], 'f', 0)),row,i+1,1,1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
    for (int i=0; i<3; ++i)
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(SWJRecord.Inaccuracy[i], 'f', 1)),row,i+1,1,1);
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

void SwitchJournalDialog::ShowOsc()
{
    EOscillogram::GBoStruct gbos = OscMap[SWJRecord.OscTime];
    EOscillogram *OscFunc = new EOscillogram;
    OscFunc->BA.resize(gbos.FileLength);
    if (Commands::GetOsc(gbos.FileNum, &(OscFunc->BA.data()[0])) == NOERROR)
    {
        QString tmps = pc.SystemHomeDir+"/temporary.osc";
        pc.SaveToFile(tmps, OscFunc->BA, gbos.FileLength);
        OscFunc->ProcessOsc();
    }
    else
        WARNMSG("");
}

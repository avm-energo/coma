#include <QVBoxLayout>
#include <QPushButton>
#include "../gen/publicclass.h"
#include "../gen/commands.h"
#include "../widgets/wd_func.h"
#include "../widgets/messagebox.h"
#include "swjdialog.h"

SWJDialog::SWJDialog(int mode, QWidget *parent) : QDialog(parent)
{
    EOscillogram *OscFunc = new EOscillogram;
}

void SWJDialog::Init(SWJournalRecordStruct &swjr, bool haveosc, EOscillogram::GBoStruct &gbos)
{
    SWJRecord = swjr;
    GBOs = gbos;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    glyout->addWidget(WDFunc::NewLBL(this, "Номер"), 0,0,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Дата, время"),0,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Аппарат"),0,2,1,2);
    glyout->addWidget(WDFunc::NewLBL(this, "Переключение"),0,4,1,2);
    glyout->addWidget(WDFunc::NewLBLT(this, QString::number(swjr.Num)), 1,0,1,1);
    glyout->addWidget(WDFunc::NewLBLT(this, pc.UnixTime64ToString(swjr.Time)),1,1,1,1);
    QStringList tmpsl = QStringList() << "D" << "G" << "CB";
    QString tmps = (swjr.TypeA < tmpsl.size()) ? tmpsl.at(swjr.TypeA) : "N/A";
    glyout->addWidget(WDFunc::NewLBLT(this, tmps),1,2,1,1);
    glyout->addWidget(WDFunc::NewLBLT(this, QString::number(swjr.NumA)),1,3,1,1);
    tmps = (swjr.Options & 0x00000001) ? "ВКЛ" : "ОТКЛ";
    glyout->addWidget(WDFunc::NewLBLT(this, tmps),1,4,1,2);
    glyout->addWidget(WDFunc::NewLBL(this, "Результат переключения"),2,0,1,6);
    glyout->addWidget(WDFunc::NewLBL(this, "Тип коммутации и коммутируемые фазы"),3,0,1,4);
    glyout->addWidget(WDFunc::NewLBLT(this, QString::number(swjr.OpResult)),3,4,1,1);
    if (haveosc)
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
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(swjr.I[i], 'f', 1)),row,i+1,1,1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
    for (int i=0; i<3; ++i)
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(swjr.U[i], 'f', 1)),row,i+1,1,1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
    for (int i=0; i<3; ++i)
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(swjr.OwnTime[i], 'f', 1)),row,i+1,1,1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
    for (int i=0; i<3; ++i)
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(swjr.FullTime[i], 'f', 1)),row,i+1,1,1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
    for (int i=0; i<3; ++i)
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(swjr.MovTime[i], 'f', 1)),row,i+1,1,1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
    for (int i=0; i<3; ++i)
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(swjr.ArchTime[i], 'f', 1)),row,i+1,1,1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
    for (int i=0; i<3; ++i)
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(swjr.IdleTime[i], 'f', 0)),row,i+1,1,1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
    for (int i=0; i<3; ++i)
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(swjr.Inaccuracy[i], 'f', 1)),row,i+1,1,1);
    vlyout->addLayout(glyout);
    QPushButton *pb = new QPushButton;
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SaveSWJ()));
    setLayout(vlyout);
}

void SWJDialog::LoadOsc(QByteArray &ba)
{
    OscFunc->BA = ba;
}

void SWJDialog::SaveSWJ()
{
    int OscTime = SWJRecord.OscTime;
    QByteArray ba;
    int SWJSize = sizeof(SWJournalRecordStruct);
    int GBOSize = sizeof(EOscillogram::GBoStruct);
    ba.resize(SWJSize + GBOSize + GBOs.FileLength);
    memcpy(&(ba.data()[0]), SWJRecord, SWJSize);
    memcpy(&(ba.data()[SWJSize]), GBOs, GBOSize);
    if (Commands::GetOsc(GBOs.FileNum, &(ba.data()[SWJSize+GBOSize])) == NOERROR)
        pc.SaveFile(this, "Файлы жуналов (*.swj)", "swj", ba, ba.size());
    else
        MessageBox2::error(this, "Ошибка", "Ошибка чтения осциллограммы из модуля");
}

void SWJDialog::ShowOsc()
{
    OscFunc->BA.resize(GBOs.FileLength);
    if (Commands::GetOsc(GBOs.FileNum, &(OscFunc->BA.data()[0])) == NOERROR)
    {
        QString tmps = pc.SystemHomeDir+"/temporary.osc";
        pc.SaveToFile(tmps, OscFunc->BA, GBOs.FileLength);
        OscFunc->ProcessOsc();
    }
    else
        WARNMSG("");
}

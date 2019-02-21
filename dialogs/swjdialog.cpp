#include <QVBoxLayout>
#include <QPushButton>
#include "../gen/stdfunc.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif
#include "../widgets/wd_func.h"
#include "../widgets/emessagebox.h"
#include "swjdialog.h"
#include "../gen/timefunc.h"
#include "../gen/error.h"
#include "../gen/files.h"

SWJDialog::SWJDialog(int mode, QWidget *parent) : QDialog(parent)
{
    Mode = mode;
    OscFunc = new EOscillogram(this);
}

void SWJDialog::Init(bool haveosc, EOscillogram::GBoStruct &gbos)
{
    GBOs = gbos;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    GetSwjOscData();
    memcpy(&SWJRecord, &(OscFunc->BA.data()[0]), sizeof(SWJournalRecordStruct));

    glyout->addWidget(WDFunc::NewLBL(this, "Номер"), 0,0,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Дата, время"),0,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Аппарат"),0,2,1,2);
    glyout->addWidget(WDFunc::NewLBL(this, "Переключение"),0,4,1,2);
    glyout->addWidget(WDFunc::NewLBLT(this, QString::number(SWJRecord.Num)), 1,0,1,1);
    glyout->addWidget(WDFunc::NewLBLT(this, TimeFunc::UnixTime64ToString(SWJRecord.Time)),1,1,1,1);
    QStringList tmpsl = QStringList() << "D" << "G" << "CB";
    QString tmps = (SWJRecord.TypeA < tmpsl.size()) ? tmpsl.at(SWJRecord.TypeA) : "N/A";
    glyout->addWidget(WDFunc::NewLBLT(this, tmps),1,2,1,1);
    glyout->addWidget(WDFunc::NewLBLT(this, QString::number(SWJRecord.NumA)),1,3,1,1);
    tmps = (SWJRecord.Options & 0x00000001) ? "ВКЛ" : "ОТКЛ";
    glyout->addWidget(WDFunc::NewLBLT(this, tmps),1,4,1,2);
    glyout->addWidget(WDFunc::NewLBL(this, "Результат переключения"),2,0,1,6);
    glyout->addWidget(WDFunc::NewLBL(this, "Тип коммутации и коммутируемые фазы"),3,0,1,4);
    glyout->addWidget(WDFunc::NewLBLT(this, QString::number(SWJRecord.OpResult)),3,4,1,1);
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
    QPushButton *pb = new QPushButton("Сохранить журнал в файл");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SaveSWJ()));
    vlyout->addWidget(pb);
    setLayout(vlyout);
}

void SWJDialog::LoadOsc(QByteArray &ba)
{
    OscFunc->BA = ba;
}

#if PROGSIZE != PROGSIZE_EMUL
void SWJDialog::SaveSWJ()
{
    QByteArray ba;
    int SWJSize = sizeof(SWJournalRecordStruct);
    int GBOSize = sizeof(EOscillogram::GBoStruct);
    ba.resize(SWJSize + GBOSize + GBOs.FileLength);
    memcpy(&(ba.data()[0]), &SWJRecord, SWJSize);
    memcpy(&(ba.data()[SWJSize]), &GBOs, GBOSize);
    if (Commands::GetOsc(GBOs.FileNum, &(ba.data()[SWJSize+GBOSize])) == Error::ER_NOERROR)
        Files::SaveToFile(Files::ChooseFileForSave(this, "Файлы жуналов (*.swj)", "swj"), ba, ba.size());
    else
        EMessageBox::error(this, "Ошибка", "Ошибка чтения осциллограммы из модуля");
}

void SWJDialog::ShowOsc()
{
}

void SWJDialog::GetSwjOscData()
{

        OscFunc->BA.resize(GBOs.FileLength);
        if (Commands::GetOsc(GBOs.FileNum, &(OscFunc->BA.data()[0])) != Error::ER_NOERROR)
        {
            WARNMSG("");
            return;
        }
        QString tmps = StdFunc::GetSystemHomeDir()+"/temporary.osc";
        Files::SaveToFile(tmps, OscFunc->BA, GBOs.FileLength);
        OscFunc->ProcessOsc();

}
#endif

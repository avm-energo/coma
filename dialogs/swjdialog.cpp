#include <QVBoxLayout>
#include <QPushButton>
#include "../gen/stdfunc.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif
#include "../models/trendviewmodel.h"
#include "../widgets/wd_func.h"
#include "../widgets/emessagebox.h"
#include "swjdialog.h"
#include "../gen/timefunc.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/colors.h"

SWJDialog::SWJDialog(int mode, QWidget *parent) : QDialog(parent)
{
    Mode = mode;
    OscFunc = new EOscillogram(this);
}

void SWJDialog::Init(SWJDialog::SWJINFStruct swj)
{
    SWJInf = swj;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    GetSwjOscData();

    glyout->addWidget(WDFunc::NewLBL(this, "Номер"), 0,0,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Дата, время"),0,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Аппарат"),0,2,1,2);
    glyout->addWidget(WDFunc::NewLBL(this, "Переключение"),0,4,1,2);
    glyout->addWidget(WDFunc::NewLBLT(this, QString::number(OscFunc->SWJRecord.Num)), 1,0,1,1);
    glyout->addWidget(WDFunc::NewLBLT(this, TimeFunc::UnixTime64ToString(OscFunc->SWJRecord.Time)),1,1,1,1);
    QStringList tmpsl = QStringList() << "D" << "G" << "CB";
    QString tmps = (OscFunc->SWJRecord.TypeA < tmpsl.size()) ? tmpsl.at(OscFunc->SWJRecord.TypeA) : "N/A";
    glyout->addWidget(WDFunc::NewLBLT(this, tmps),1,2,1,1);
    glyout->addWidget(WDFunc::NewLBLT(this, QString::number(OscFunc->SWJRecord.NumA)),1,3,1,1);
    tmps = (OscFunc->SWJRecord.Options & 0x00000001) ? "ВКЛ" : "ОТКЛ";
    glyout->addWidget(WDFunc::NewLBLT(this, tmps),1,4,1,2);
    glyout->addWidget(WDFunc::NewLBL(this, "Результат переключения"),2,0,1,6);
    glyout->addWidget(WDFunc::NewLBL(this, "Тип коммутации и коммутируемые фазы"),3,0,1,4);
    glyout->addWidget(WDFunc::NewLBLT(this, QString::number(OscFunc->SWJRecord.OpResult)),3,4,1,1);
    if (SWJInf.FileLength)
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
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(OscFunc->SWJRecord.I[i], 'f', 1)),row,i+1,1,1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
    for (int i=0; i<3; ++i)
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(OscFunc->SWJRecord.U[i], 'f', 1)),row,i+1,1,1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
    for (int i=0; i<3; ++i)
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(OscFunc->SWJRecord.OwnTime[i], 'f', 1)),row,i+1,1,1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
    for (int i=0; i<3; ++i)
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(OscFunc->SWJRecord.FullTime[i], 'f', 1)),row,i+1,1,1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
    for (int i=0; i<3; ++i)
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(OscFunc->SWJRecord.MovTime[i], 'f', 1)),row,i+1,1,1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
    for (int i=0; i<3; ++i)
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(OscFunc->SWJRecord.ArchTime[i], 'f', 1)),row,i+1,1,1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
    for (int i=0; i<3; ++i)
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(OscFunc->SWJRecord.IdleTime[i], 'f', 0)),row,i+1,1,1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
    for (int i=0; i<3; ++i)
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(OscFunc->SWJRecord.Inaccuracy[i], 'f', 1)),row,i+1,1,1);
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
    ba.resize(SWJInf.FileLength);
    memcpy(&(ba.data()[0]), &OscFunc->SWJRecord, SWJSize);
    //memcpy(&(ba.data()[SWJSize]), &SWJInf, GBOSize);
    if (Commands::GetOsc(SWJInf.FileNum, &(ba.data()[SWJSize+GBOSize])) == Error::ER_NOERROR)
        Files::SaveToFile(Files::ChooseFileForSave(this, "Файлы жуналов (*.swj)", "swj"), ba, ba.size());
    else
        EMessageBox::error(this, "Ошибка", "Ошибка чтения осциллограммы из модуля");
}

void SWJDialog::ShowOsc()
{
    dlg->PlotShow();
    dlg->show();
}

void SWJDialog::GetSwjOscData()
{
    QStringList tmpav, tmpdv;
    quint32 len = 0;
    //char *ptr;

        OscFunc->BA.resize(SWJInf.FileLength + sizeof(S2::FileHeader));
        if (Commands::GetOsc(SWJInf.FileNum, &(OscFunc->BA.data()[0])) != Error::ER_NOERROR)
        {
            WARNMSG("");
            return;
        }
        QString tmps = StdFunc::GetSystemHomeDir()+"/temporary.osc";
        Files::SaveToFile(tmps, OscFunc->BA, SWJInf.FileLength);
        dlg = new TrendViewDialog(OscFunc->BA);
        mdl = new TrendViewModel(QStringList(), QStringList(), len);
        OscFunc->ProcessOsc(mdl);
        mdl->xmax = (static_cast<float>((mdl->Len)/2));
        mdl->xmin = -mdl->xmax;
        dlg->TrendModel = mdl;

        switch(mdl->idOsc)
        {
          case MT_ID85:
          {

            dlg->SetAnalogNames(mdl->tmpav_85);
            dlg->SetDigitalNames(mdl->tmpdv_85);
            dlg->SetDigitalColors(mdl->dcolors_85);
            dlg->SetAnalogColors(mdl->acolors_85);
            dlg->SetRanges(mdl->xmin, mdl->xmax, -200, 200);
            break;
          }
          case MT_ID80:
          {
            mdl->tmpdv_80.clear();
            dlg->SetAnalogNames(mdl->tmpav_80);
            dlg->SetDigitalNames(mdl->tmpdv_80);
            dlg->SetDigitalColors(mdl->dcolors_80);
            dlg->SetAnalogColors(mdl->acolors_80);
            dlg->SetRanges(mdl->xmin, mdl->xmax, -200, 200);
            break;
          }

         case MT_ID21:
         {
            // период отсчётов - 20 мс, длительность записи осциллограммы 10 сек, итого 500 точек по 4 байта на каждую
            mdl->tmpav_21 << QString::number(mdl->idOsc); // пока сделано для одного канала в осциллограмме
            //TrendViewModel *TModel = new TrendViewModel(QStringList(), tmpav, *len);
            //dlg->SetModel(TModel);
            dlg->SetAnalogNames(mdl->tmpav_21);
            dlg->SetRanges(0, 10000, -20, 20); // 10000 мс, 20 мА (сделать автонастройку в зависимости от конфигурации по данному каналу)

           break;
         }

         case ID_OSC_CH0:
         case ID_OSC_CH0+1:
         case ID_OSC_CH0+2:
         case ID_OSC_CH0+3:
         case ID_OSC_CH0+4:
         case ID_OSC_CH0+5:
         case ID_OSC_CH0+6:
         case ID_OSC_CH0+7:
         {

           dlg->SetAnalogNames(mdl->tmpav_85);
           dlg->SetDigitalNames(mdl->tmpdv_85);
           dlg->SetDigitalColors(mdl->dcolors_85);
           dlg->SetAnalogColors(mdl->acolors_85);
           dlg->SetRanges(mdl->xmin, mdl->xmax, -200, 200);
           break;
         }


        }

            dlg->SetupPlots();
            dlg->SetupUI();
            dlg->setModal(false);


}
#endif

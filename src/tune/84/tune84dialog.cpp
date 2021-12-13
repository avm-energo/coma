#include "tune84dialog.h"

#include "../../gen/colors.h"
#include "../../gen/error.h"
#include "../../gen/files.h"
#include "../../gen/stdfunc.h"
#include "../../widgets/wd_func.h"
#include "../tunesteps.h"

Tune84Dialog::Tune84Dialog(QWidget *parent) : GeneralTuneDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    m_dialogList = { { "Проверка правильности измерения входных сигналов", new Tune84Check(TS84_CHECKING, this) },
        { "Регулировка каналов напряжения", new Tune84ADC(TS84_ADCU, this) },
        { "Регулировка каналов тока", new Tune84ADC(TS84_ADCI, this) },
        { "Настройка температурной коррекции +60 °С", new TuneKIVTemp60(TS84_60TUNING, this) },
        { "Настройка температурной коррекции -20 °С", new TuneKIVTemp60(TS84_20TUNING, this) } };
    m_calibrSteps = m_dialogList.size() + 1;
    Bac *bac = new Bac;
    m_BacWidget = bac->widget();
    SetupUI();
}

void Tune84Dialog::prepareReport()
{
    /*    // данные в таблицу уже получены или из файла, или в процессе работы
        // отобразим таблицу
       // ShowTable();
       // QString GOST = (PovType == GOST_1983) ? "1983" : "23625";
        report = new LimeReport::ReportEngine(this);
        QString path = StdFunc::GetSystemHomeDir()+"82report.lrxml";
        report->loadFromFile(path);
        report->dataManager()->addModel("maindata", ReportModel, false);

        ReportHeader.Organization = "ООО АСУ-ВЭИ";
        report->dataManager()->setReportVariable("Organization", ReportHeader.Organization);
        QString day = QDateTime::currentDateTime().toString("dd");
        QString month = QDateTime::currentDateTime().toString("MM");
        QString yr = QDateTime::currentDateTime().toString("yy");
        report->dataManager()->setReportVariable("Day", day);
        report->dataManager()->setReportVariable("Month", month);
        report->dataManager()->setReportVariable("Yr", yr);

        for(int i=0; i<21; i++) // 21 таблица!
        {
            if(i==0)
            {
                if (Commands::GetFile(CM_CONFIGFILE,S2ConfigForTune) == NOERROR)
                {
                   WaitNSeconds(1);
                   Start7_3_7_2();  // Переход на конфигурацию 1А
                }
            }

            if(i==6)
            {
                if (Commands::GetFile(CM_CONFIGFILE,S2ConfigForTune) == NOERROR)
                {
                   WaitNSeconds(1);
                   Start7_3_7_6();  // Переход на конфигурацию 5А
                }
            }

                QDialog *dlg = new QDialog;
                QVBoxLayout *lyout = new QVBoxLayout;
                QLabel *lbl = new QLabel;
                lbl=new QLabel("Задайте на РЕТОМ трёхфазный режим токов и напряжений (Uabc, Iabc) с углами "\
                               "нагрузки по всем фазам " +QString::number(PhiLoad[i])+ " град. и частотой 51 Гц;");
                lyout->addWidget(lbl);
                lbl=new QLabel("Значения напряжений по фазам " +QString::number(U[i])+ " В;");
                lyout->addWidget(lbl);
                if (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) != Config::MTM_83)
                {
                    lbl=new QLabel("Значения токов по фазам " +QString::number(I[i])+ " А;");
                    lyout->addWidget(lbl);
                }
                QPushButton *pb = new QPushButton("Готово");
                connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
                lyout->addWidget(pb);
                pb = new QPushButton("Отмена");
                connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
                connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
                lyout->addWidget(pb);
                dlg->setLayout(lyout);
                dlg->exec();

               if(Cancelled)
               break;

            TuneControlType = 0;
            GetExternalData();
            ReadAnalogMeasurements();
            WaitNSeconds(1);
            FillBd1(this);

            if(PhiLoad[i] >= 180)
            {
                ReportHeader.PhiloadA = QString::number(360 + Bda_block.phi_next_f[3], 'f', 3);
                RealData.d[0] = 360 - RealData.d[0];
                RealData.d[1] = 360 - RealData.d[1];
                RealData.d[2] = 360 - RealData.d[2];
            }
            else
            {
                ReportHeader.PhiloadA = QString::number(Bda_block.phi_next_f[3], 'f', 3);
            }

            ReportHeader.PhiloadB = QString::number(Bda_block.phi_next_f[4] - Bda_block.phi_next_f[1], 'f', 3);
            if(PhiLoad[i] >= 90)
            {
               ReportHeader.PhiloadC = QString::number(360 + Bda_block.phi_next_f[5] - Bda_block.phi_next_f[2], 'f', 3);
            }
            else
            {
               ReportHeader.PhiloadC = QString::number(Bda_block.phi_next_f[5] - Bda_block.phi_next_f[2], 'f', 3);
            }
            ReportHeader.PhiUAB   = QString::number((-Bda_block.phi_next_f[1]), 'f', 3);
            ReportHeader.PhiUBC   = QString::number((360 - Bda_block.phi_next_f[2] + Bda_block.phi_next_f[1]), 'f', 3);
            ReportHeader.OffsetF  = QString::number(100*((Bda_block.Frequency/RealData.f[0])-1), 'f', 3);
            ReportHeader.OffsetUA = QString::number(100*((Bda_block.IUefNat_filt[0]/RealData.u[0])-1), 'f', 3);
            ReportHeader.OffsetUB = QString::number(100*((Bda_block.IUefNat_filt[1]/RealData.u[1])-1), 'f', 3);
            ReportHeader.OffsetUC = QString::number(100*((Bda_block.IUefNat_filt[2]/RealData.u[2])-1), 'f', 3);
            ReportHeader.OffsetIA = QString::number(100*((Bda_block.IUefNat_filt[3]/RealData.i[0])-1), 'f', 3);
            ReportHeader.OffsetIB = QString::number(100*((Bda_block.IUefNat_filt[4]/RealData.i[1])-1), 'f', 3);
            ReportHeader.OffsetIC = QString::number(100*((Bda_block.IUefNat_filt[5]/RealData.i[2])-1), 'f', 3);

            // Играемся с углами, чтобы все было в одних значениях и с одинаковыми знаками
            if((RealData.d[0]>0 && ReportHeader.PhiloadA.toFloat() < 0) || (RealData.d[0]<0 &&
       ReportHeader.PhiloadA.toFloat() > 0))
            {
               ReportHeader.OffsetPhiloadA = QString::number(RealData.d[0] + ReportHeader.PhiloadA.toFloat(), 'f', 3);
               RealData.d[0] = -RealData.d[0];
            }
            else
               ReportHeader.OffsetPhiloadA = QString::number(RealData.d[0] - ReportHeader.PhiloadA.toFloat(), 'f', 3);

            if((RealData.d[1]>0 && ReportHeader.PhiloadB.toFloat() < 0) || (RealData.d[1]<0 &&
       ReportHeader.PhiloadB.toFloat() > 0))
            {
               ReportHeader.OffsetPhiloadB = QString::number(RealData.d[1] + ReportHeader.PhiloadB.toFloat(), 'f', 3);
               RealData.d[1] = -RealData.d[1];
            }
            else
               ReportHeader.OffsetPhiloadB = QString::number(RealData.d[1] - ReportHeader.PhiloadB.toFloat(), 'f', 3);

            if((RealData.d[2]>0 && ReportHeader.PhiloadC.toFloat() < 0) || (RealData.d[2]<0 &&
       ReportHeader.PhiloadC.toFloat() > 0))
            {
               ReportHeader.OffsetPhiloadC = QString::number(RealData.d[2] + ReportHeader.PhiloadC.toFloat(), 'f', 3);
               RealData.d[2] = -RealData.d[2];
            }
            else
               ReportHeader.OffsetPhiloadC = QString::number(RealData.d[2] - ReportHeader.PhiloadC.toFloat(), 'f', 3);

            ReportHeader.OffsetPhiUAB = QString::number(RealData.dpsiU[0] - ReportHeader.PhiUAB.toFloat(), 'f', 3);
            ReportHeader.OffsetPhiUBC = QString::number(RealData.dpsiU[1] - ReportHeader.PhiUBC.toFloat(), 'f', 3);

            report->dataManager()->setReportVariable("FreqMIP", QString::number(RealData.f[0], 'f', 3));
            report->dataManager()->setReportVariable("UA_MIP."+QString::number(i), QString::number(RealData.u[0], 'f',
       3)); report->dataManager()->setReportVariable("UB_MIP."+QString::number(i), QString::number(RealData.u[1], 'f',
       3)); report->dataManager()->setReportVariable("UC_MIP."+QString::number(i), QString::number(RealData.u[2], 'f',
       3)); report->dataManager()->setReportVariable("IA_MIP."+QString::number(i), QString::number(RealData.i[0], 'f',
       3)); report->dataManager()->setReportVariable("IB_MIP."+QString::number(i), QString::number(RealData.i[1], 'f',
       3)); report->dataManager()->setReportVariable("IC_MIP."+QString::number(i), QString::number(RealData.i[2], 'f',
       3)); report->dataManager()->setReportVariable("PhiLA_MIP."+QString::number(i), QString::number(RealData.d[0],
       'f', 3)); report->dataManager()->setReportVariable("PhiLB_MIP."+QString::number(i),
       QString::number(RealData.d[1], 'f', 3));
       report->dataManager()->setReportVariable("PhiLC_MIP."+QString::number(i), QString::number(RealData.d[2], 'f',
       3)); report->dataManager()->setReportVariable("PhiUab_MIP."+QString::number(i),
       QString::number(RealData.dpsiU[0], 'f', 3));
       report->dataManager()->setReportVariable("PhiUbc_MIP."+QString::number(i), QString::number(RealData.dpsiU[1],
       'f', 3)); report->dataManager()->setReportVariable("Freq."+QString::number(i),
       QString::number(Bda_block.Frequency, 'f', 3)); report->dataManager()->setReportVariable("UA."+QString::number(i),
       QString::number(Bda_block.IUefNat_filt[0], 'f', 3));
            report->dataManager()->setReportVariable("UB."+QString::number(i),
       QString::number(Bda_block.IUefNat_filt[1], 'f', 3));
       report->dataManager()->setReportVariable("UC."+QString::number(i), QString::number(Bda_block.IUefNat_filt[2],
       'f', 3)); report->dataManager()->setReportVariable("IA."+QString::number(i),
       QString::number(Bda_block.IUefNat_filt[3], 'f', 3));
       report->dataManager()->setReportVariable("IB."+QString::number(i), QString::number(Bda_block.IUefNat_filt[4],
       'f', 3)); report->dataManager()->setReportVariable("IC."+QString::number(i),
       QString::number(Bda_block.IUefNat_filt[5], 'f', 3));
       report->dataManager()->setReportVariable("PhiLA."+QString::number(i), ReportHeader.PhiloadA);
            report->dataManager()->setReportVariable("PhiLB."+QString::number(i), ReportHeader.PhiloadB);
            report->dataManager()->setReportVariable("PhiLC."+QString::number(i), ReportHeader.PhiloadC);
            report->dataManager()->setReportVariable("PhiUab."+QString::number(i), ReportHeader.PhiUAB);
            report->dataManager()->setReportVariable("PhiUbc."+QString::number(i), ReportHeader.PhiUBC);
            report->dataManager()->setReportVariable("OffsetF."+QString::number(i), ReportHeader.OffsetF);
            report->dataManager()->setReportVariable("OffsetUA."+QString::number(i), ReportHeader.OffsetUA);
            report->dataManager()->setReportVariable("OffsetUB."+QString::number(i), ReportHeader.OffsetUB);
            report->dataManager()->setReportVariable("OffsetUC."+QString::number(i), ReportHeader.OffsetUC);
            report->dataManager()->setReportVariable("OffsetIA."+QString::number(i), ReportHeader.OffsetIA);
            report->dataManager()->setReportVariable("OffsetIB."+QString::number(i), ReportHeader.OffsetIB);
            report->dataManager()->setReportVariable("OffsetIC."+QString::number(i), ReportHeader.OffsetIC);
            report->dataManager()->setReportVariable("OffsetPhiloadA."+QString::number(i), ReportHeader.OffsetPhiloadA);
            report->dataManager()->setReportVariable("OffsetPhiloadB."+QString::number(i), ReportHeader.OffsetPhiloadB);
            report->dataManager()->setReportVariable("OffsetPhiloadC."+QString::number(i), ReportHeader.OffsetPhiloadC);
            report->dataManager()->setReportVariable("OffsetPhiUAB."+QString::number(i), ReportHeader.OffsetPhiUAB);
            report->dataManager()->setReportVariable("OffsetPhiUBC."+QString::number(i), ReportHeader.OffsetPhiUBC);

        }

        if (QMessageBox::question(this,"Сохранить","Сохранить протокол поверки?"))
        {
            QString filename = Files::ChooseFileForSave(this, "*.pdf", "pdf");
            if (!filename.isEmpty())
            {
                report->designReport();
                report->printToPDF(filename);
        //        report->previewReport();
              //  report->designReport();
                QMessageBox::information(this, "Успешно!", "Записано успешно!");
            }
            else
                QMessageBox::information(this, "Отменено", "Действие отменено");
        }
        delete report; */
}

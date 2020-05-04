#include <QGroupBox>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QApplication>
#include <QFileDialog>
#include "../widgets/emessagebox.h"
#include "../gen/maindef.h"
#include "../widgets/getoscpbdelegate.h"
#include "../widgets/wd_func.h"
#include "../gen/stdfunc.h"
#include "../gen/error.h"
#include "../gen/timefunc.h"
#include "../dialogs/journalsdialog.h"
#include "../gen/colors.h"
#include "../gen/mainwindow.h"
#include "../gen/files.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif


JournalDialog::JournalDialog() : QDialog()
{
    SaveMJour = nullptr;
    SaveWJour = nullptr;
    SaveSJour = nullptr;
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

void JournalDialog::SetupUI()
{
    //QWidget *work = new QWidget;
    //QWidget *system = new QWidget;

    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    ETableView * tv = new ETableView;
    tv->setObjectName("work");
    TableSysModel = new ETableModel;
    TableWorkModel = new ETableModel;
    TableMeasModel = new ETableModel;

    //int i;
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    //QTabWidget *ConfTW = new QTabWidget;
    QTabWidget *work = new QTabWidget;
    //work->setObjectName("conftw1");
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    work->tabBar()->setStyleSheet(ConfTWss);

    //QGroupBox *gb = new QGroupBox("Системный журнал");
    //QVBoxLayout *gblyout = new QVBoxLayout;

    QPushButton *pb = new QPushButton("Получить рабочий журнал");
    connect(pb,SIGNAL(clicked()),this,SLOT(GetWorkJour()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    pb = new QPushButton("Стереть рабочий журнал");
    //connect(pb,SIGNAL(clicked()),this,SLOT(EraseJournals()));
    if(StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    pb = new QPushButton("Сохранить журнал в файл");
    connect(pb,SIGNAL(clicked()),this,SLOT(SaveWorkToTXTFile()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    vlyout->addLayout(hlyout);
    vlyout->addWidget(tv, 89);

    work->setLayout(vlyout);
    work->setStyleSheet(ConfTWss);

    QTabWidget *system = new QTabWidget;
    //system->setObjectName("system");
    system->tabBar()->setStyleSheet(ConfTWss);

    hlyout = new QHBoxLayout;
    vlyout = new QVBoxLayout;

    tv = new ETableView;
    tv->setObjectName("system");

    pb = new QPushButton("Получить системный журнал");
    connect(pb,SIGNAL(clicked()),this,SLOT(GetSystemJour()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    pb = new QPushButton("Стереть системный журнал");
    //connect(pb,SIGNAL(clicked()),this,SLOT(EraseJournals()));
    if(StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    pb = new QPushButton("Сохранить журнал в файл");
    connect(pb,SIGNAL(clicked()),this,SLOT(SaveSysToTXTFile()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    vlyout->addLayout(hlyout);
    vlyout->addWidget(tv, 89);

    system->setLayout(vlyout);
    system->setStyleSheet(ConfTWss);

    QTabWidget *measure = new QTabWidget;
    //measure->setObjectName("meas");
    measure->tabBar()->setStyleSheet(ConfTWss);
    hlyout = new QHBoxLayout;
    vlyout = new QVBoxLayout;

    tv = new ETableView;
    tv->setObjectName("meas");

    pb = new QPushButton("Получить журнал измерений");
    connect(pb,SIGNAL(clicked()),this,SLOT(GetMeasJour()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    pb = new QPushButton("Стереть журнал измерений");
    //connect(pb,SIGNAL(clicked()),this,SLOT(EraseJournals()));
    if(StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    pb = new QPushButton("Сохранить журнал в файл");
    connect(pb,SIGNAL(clicked()),this,SLOT(SaveMeasToTXTFile()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    vlyout->addLayout(hlyout);
    vlyout->addWidget(tv, 89);

    measure->setLayout(vlyout);
    measure->setStyleSheet(ConfTWss);

    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw4");
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(work,"Рабочий журнал");
    ConfTW->addTab(system,"Системный журнал");
    ConfTW->addTab(measure,"Журнал измерений");

    lyout->addWidget(ConfTW);
    setLayout(lyout);
}

void JournalDialog::ProcessSWJournal(QByteArray &ba)
{

}

#if PROGSIZE != PROGSIZE_EMUL
void JournalDialog::GetSystemJour()
{
    //char* num = new char;
    //*num = 4;
    QVector<S2::DataRec>* Jour = new QVector<S2::DataRec>;
    char num = 4;
    if(MainWindow::MainInterface == "Ethernet")
    {
      emit ReadJour(&num);
    }
    else if(MainWindow::MainInterface == "USB")
    {
      Jour->resize(1);
      if(Commands::GetJour(num, &(Jour->data()[0])) == Error::ER_NOERROR)
      {
         FillSysJour(Jour);
      }
      else
      {
        EMessageBox::information(this, "Ошибка", "Ошибка чтения журнала");
      }
    }

}

void JournalDialog::GetWorkJour()
{
    //char* num = new char;
    //*num = 4;
    QVector<S2::DataRec> *Jour  = new QVector<S2::DataRec>;
    char num = 5;

    if(MainWindow::MainInterface == "Ethernet")
    {
      emit ReadJour(&num);
    }
    else if(MainWindow::MainInterface == "USB")
    {
      Jour->resize(1);
      if(Commands::GetJour(num, &(Jour->data()[0])) == Error::ER_NOERROR)
      {
         FillWorkJour(Jour);
      }
      else
      {
         EMessageBox::information(this, "Ошибка", "Ошибка чтения журнала");
      }
    }

}

void JournalDialog::GetMeasJour()
{
    //char* num = new char;
    //*num = 4;    
    QVector<S2::DataRec>* Jour = new QVector<S2::DataRec>;
    char num = 6;

    if(MainWindow::MainInterface == "Ethernet")
    {
      emit ReadJour(&num);
    }
    else if(MainWindow::MainInterface == "USB")
    {
      Jour->resize(1);
      if(Commands::GetJour(num, &(Jour->data()[0])) == Error::ER_NOERROR)
      {
         FillMeasJour(Jour);
      }
      else
      {
         EMessageBox::information(this, "Ошибка", "Ошибка чтения журнала");
      }
    }


}

void JournalDialog::FillSysJour(QVector<S2::DataRec>* File)
{
    QVector<QVector<QVariant> > lsl;
    QByteArray OscInfo;
    char *mem;
    int N = 0;
    int JourSize = 0; // размер считанного буфера с информацией
    int RecordSize = sizeof(SystemWorkStruct);
    SystemWorkStruct System;
    //quint32 size = File->at(0).num_byte;
    SaveSJour = new QVector<S2::DataRec>;
    SaveSJour->resize(1);
    memcpy((char*)(&SaveSJour->data()[0]), (char*)&File->data()[0], 12);
    size_t tmpt = static_cast<size_t>(RecordSize);
    QStringList Discription = QStringList() << "Произошёл рестарт программного обеспечения модуля"
                                            << "Произошла запись и переход на новую конфигурацию"
                                            << "Произошла запись и переход на новую версию ВПО"
                                            << "Произошла запись в блок Bhb (hidden block)"
                                            << "Произошла запись в блок Bhbm (hidden block)( Мезонин)"
                                            << "Произошёл отказ внешней Flash-памяти 4 Мбайт на базовой плате"
                                            << "Произошёл отказ внешней Flash-памяти 512 байт на мезонинной плате"
                                            << "Ошибка работы внешней FRAM памяти"
                                            << "Произошёл отказ при обновлении конфигурации"
                                            << "Ошибка загрузки конфигурации из flash памяти"
                                            << "Ошибка регулировочных коэффициентов (базовая)"
                                            << "Ошибка регулировочных коэффициентов (Мезонин)"
                                            << "Ошибка перегрев модуля"
                                            << "Напряжение батарейки низкое (< 2,5 В)"
                                            << "Ошибка связи с NTP сервером"
                                            << "Ошибка связи с 1PPS от антенны"
                                            << "Ошибка АЦП (Мезонин)"
                                            << "Ошибка АЦП (базовая)"
                                            << "Произошла запись регулировочные коэффициенты (базовая)"
                                            << "Произошла запись регулировочные коэффициенты (Мезонин)"
                                            << "Произошло стирание системного журнала"
                                            << "Произошло стирание рабочего журнала"
                                            << "Произошло стирание осциллограмм"
                                            << "Произошло стирание журнала измерений"
                                            << "Ошибка ВПО"
                                            << "Ошибка встроенного АЦП"
                                            << "Произошла запись в блок Bhb (hidden block)"
                                            << "Произошла запись в блок Bhbm (hidden block)( Мезонин)";

    memcpy(&JourSize, &(File->at(0).num_byte), 4);

        TableSysModel->ClearModel();
        TableSysModel->addColumn(" № ");
        TableSysModel->addColumn("Описание события");
        TableSysModel->addColumn("Дата/Время");
        TableSysModel->addColumn("Тип события");

        QApplication::setOverrideCursor(Qt::WaitCursor);
        QVector<QVariant> EventNum, Num, Time, Type;
        int counter = 0;

        for(int i = 0; i < JourSize; i+= RecordSize)
        {
            mem = static_cast<char *>(File->at(0).thedata);
            mem +=i;
            memcpy(&System, mem, tmpt);

            if(System.Time != 0xFFFFFFFFFFFFFFFF)
            {
                if(LTime > System.Time)
                {
                  LTime = System.Time;
                  SaveI = i;
                }
            }
        }

        for (int i = SaveI; i < JourSize; i+= RecordSize)
        {
            mem = static_cast<char *>(File->at(0).thedata);
            mem +=i;
            memcpy(&System, mem, tmpt);

            if(System.Time != 0xFFFFFFFFFFFFFFFF)
            {
                memcpy(&N, &System.EvNum, sizeof(System.EvNum));
                N = N&0x00FFFFFF;

                if(N <= Discription.size())
                {
                  Num << Discription.at(N-1);
                }
                else
                {
                  Num << "Некорректный номер события";
                }

                Time << TimeFunc::UnixTime64ToString(System.Time);

                if(System.EvType)
                Type << "Пришло";
                else
                Type << "Ушло";

                ++counter;
                EventNum << counter;
            }

        }

        if(SaveI != 0)
        {
            for (int i = 0; i < (SaveI); i+= RecordSize)
            {
                mem = static_cast<char *>(File->at(0).thedata);
                mem +=i;
                memcpy(&System, mem, tmpt);

                if(System.Time != 0xFFFFFFFFFFFFFFFF)
                {
                    memcpy(&N, &System.EvNum, sizeof(System.EvNum));
                    N = N&0x00FFFFFF;

                    if(N <= Discription.size())
                    {
                      Num << Discription.at(N-1);
                    }
                    else
                    {
                      Num << "Некорректный номер события";
                    }

                    Time << TimeFunc::UnixTime64ToString(System.Time);

                    if(System.EvType)
                    Type << "Пришло";
                    else
                    Type << "Ушло";

                    ++counter;
                    EventNum << counter;
                }

            }
        }

        lsl.append(EventNum);
        lsl.append(Num);
        lsl.append(Time);
        lsl.append(Type);
        TableSysModel->fillModel(lsl);
        UpdateSysModel();
        QApplication::restoreOverrideCursor();

}



void JournalDialog::FillWorkJour(QVector<S2::DataRec>* File)
{
    QVector<QVector<QVariant> > lsl;
    QByteArray OscInfo;
    char *mem;
    int N = 0;
    int JourSize = 0; // размер считанного буфера с информацией
    int RecordSize = sizeof(SystemWorkStruct);
    SystemWorkStruct System;
    //quint32 size = File->at(0).num_byte;
    SaveWJour = new QVector<S2::DataRec>;
    SaveWJour->resize(1);
    memcpy((char*)(&SaveWJour->data()[0]), (char*)&File->data()[0], 12);
    size_t tmpt = static_cast<size_t>(RecordSize);
    QStringList Discription = QStringList() << "Отсутствует сигнал напряжения фазы A"
                                            << "Отсутствует сигнал напряжения фазы B"
                                            << "Отсутствует сигнал напряжения фазы С"
                                            << "Нет реактивного тока канала А (ток меньше 2мА)"
                                            << "Нет реактивного тока канала B (ток меньше 2мА)"
                                            << "Нет реактивного тока канала C (ток меньше 2мА)"
                                            << "Не заданы начальные значения"
                                            << "Низкое напряжение фазы A"
                                            << "Низкое напряжение фазы B"
                                            << "Низкое напряжение фазы C"
                                            << "Сигнализация по приращению тангенса дельта ввода фазы А"
                                            << "Сигнализация по приращению тангенса дельта ввода фазы B"
                                            << "Сигнализация по приращению тангенса дельта ввода фазы C"
                                            << "Авария по приращению тангенса дельта ввода фазы А"
                                            << "Авария по приращению тангенса дельта ввода фазы B"
                                            << "Авария по приращению тангенса дельта ввода фазы C"
                                            << "Сигнализация по приращению C ввода фазы А"
                                            << "Сигнализация по приращению C ввода фазы B"
                                            << "Сигнализация по приращению C ввода фазы C"
                                            << "Авария по приращению C ввода фазы А"
                                            << "Авария по приращению C ввода фазы B"
                                            << "Авария по приращению C ввода фазы C";

    memcpy(&JourSize, &(File->at(0).num_byte), 4);

        TableWorkModel->ClearModel();
        TableWorkModel->addColumn(" № ");
        TableWorkModel->addColumn("Описание события");
        TableWorkModel->addColumn("Дата/Время");
        TableWorkModel->addColumn("Тип события");

        QApplication::setOverrideCursor(Qt::WaitCursor);
        QVector<QVariant> EventNum, Num, Time, Type;
        int counter = 0;

        for(int i = 0; i < JourSize; i+= RecordSize)
        {
            mem = static_cast<char *>(File->at(0).thedata);
            mem +=i;
            memcpy(&System, mem, tmpt);

            if(System.Time != 0xFFFFFFFFFFFFFFFF)
            {
                if(LTime > System.Time)
                {
                  LTime = System.Time;
                  SaveI = i;
                }
            }
        }

        for (int i = SaveI; i < JourSize; i+= RecordSize)
        {
            mem = static_cast<char *>(File->at(0).thedata);
            mem +=i;
            memcpy(&System, mem, tmpt);

            if(System.Time != 0xFFFFFFFFFFFFFFFF)
            {
                memcpy(&N, &System.EvNum, sizeof(System.EvNum));
                N = N&0x00FFFFFF;

                if((N-3010) <= Discription.size())
                {
                  Num << Discription.at(N-3011);
                }
                else
                {
                  Num << "Некорректный номер события";
                }

                Time << TimeFunc::UnixTime64ToString(System.Time);

                if(System.EvType)
                Type << "Пришло";
                else
                Type << "Ушло";

                ++counter;
                EventNum << counter;
            }

        }

        if(SaveI != 0)
        {
            for (int i = 0; i < (SaveI); i+= RecordSize)
            {
                mem = static_cast<char *>(File->at(0).thedata);
                mem +=i;
                memcpy(&System, mem, tmpt);

                if(System.Time != 0xFFFFFFFFFFFFFFFF)
                {
                    memcpy(&N, &System.EvNum, sizeof(System.EvNum));
                    N = N&0x00FFFFFF;

                    if((N-3010) <= Discription.size())
                    {
                      Num << Discription.at(N-3011);
                    }
                    else
                    {
                      Num << "Некорректный номер события";
                    }

                    Time << TimeFunc::UnixTime64ToString(System.Time);

                    if(System.EvType)
                    Type << "Пришло";
                    else
                    Type << "Ушло";

                    ++counter;
                    EventNum << counter;
                }

            }
        }

        lsl.append(EventNum);
        lsl.append(Num);
        lsl.append(Time);
        lsl.append(Type);
        TableWorkModel->fillModel(lsl);
        UpdateWorkModel();
        QApplication::restoreOverrideCursor();

}

void JournalDialog::FillMeasJour(QVector<S2::DataRec>* File)
{
    QVector<QVector<QVariant> > lsl;
    QByteArray OscInfo;
    char *mem;
    //int N = 0;
    int JourSize = 0; // размер считанного буфера с информацией
    int RecordSize = sizeof(MeasureStruct);
    MeasureStruct Meas;
    //quint32 size = File->at(0).num_byte;
    SaveMJour = new QVector<S2::DataRec>;
    SaveMJour->resize(1);
    memcpy((char*)(&SaveMJour->data()[0]), (char*)&File->data()[0], 12);
    size_t tmpt = static_cast<size_t>(RecordSize);

    memcpy(&JourSize, &(File->at(0).num_byte), 4);

        TableMeasModel->ClearModel();
        TableMeasModel->addColumn("Номер события");
        TableMeasModel->addColumn("Дата/Время");
        TableMeasModel->addColumn("Действующее значение напряжения фазы А, кВ");
        TableMeasModel->addColumn("Действующее значение напряжения фазы B, кВ");
        TableMeasModel->addColumn("Действующее значение напряжения фазы C, кВ");
        TableMeasModel->addColumn("Действующее значение тока фазы А, мА");
        TableMeasModel->addColumn("Действующее значение тока фазы B, мА");
        TableMeasModel->addColumn("Действующее значение тока фазы C, мА");
        TableMeasModel->addColumn("Напряжение нулевой последовательности");
        TableMeasModel->addColumn("Напряжение прямой последовательности");
        TableMeasModel->addColumn("Напряжение обратной последовательности");
        TableMeasModel->addColumn("Ток нулевой последовательности");
        TableMeasModel->addColumn("Ток прямой последовательности");
        TableMeasModel->addColumn("Ток обратной последовательности");
        TableMeasModel->addColumn("Емкость ввода фазы А, пФ");
        TableMeasModel->addColumn("Емкость ввода фазы B, пФ");
        TableMeasModel->addColumn("Емкость ввода фазы C, пФ");
        TableMeasModel->addColumn("tg delta ввода фазы А, %");
        TableMeasModel->addColumn("tg delta ввода фазы B, %");
        TableMeasModel->addColumn("tg delta ввода фазы C, %");
        TableMeasModel->addColumn("Изменение емкости ввода фазы А, % от C_init");
        TableMeasModel->addColumn("Изменение емкости ввода фазы B, % от C_init");
        TableMeasModel->addColumn("Изменение емкости ввода фазы C, % от C_init");
        TableMeasModel->addColumn("Изменение tg delta ввода фазы А, %");
        TableMeasModel->addColumn("Изменение tg delta ввода фазы B, %");
        TableMeasModel->addColumn("Изменение tg delta ввода фазы C, %");
        TableMeasModel->addColumn("Действующее значение 1-й гармоники тока небаланса, мА");
        TableMeasModel->addColumn("Угол тока небаланса относительно тока ф.А, град");
        TableMeasModel->addColumn("Температура кристалла микроконтроллера");
        TableMeasModel->addColumn("Температура окружающей среды");

        QApplication::setOverrideCursor(Qt::WaitCursor);
        QVector<QVariant> EventNum, Time, UeffA, UeffB, UeffC, IeffA, IeffB, IeffC, U0, U1, U2,
                          I0, I1, I2, CbushA, CbushB, CbushC, Tg_dA, Tg_dB, Tg_dC, dCbushA, dCbushB,
                          dCbushC, dTg_dA, dTg_dB, dTg_dC, Iunb, Phy_unb, Tmk, Tokr;
        //int counter = 0;

        for(int i = 0; i < JourSize; i+= RecordSize)
        {
            mem = static_cast<char *>(File->at(0).thedata);
            mem +=i;
            memcpy(&Meas, mem, tmpt);

            if(Meas.Time != 0xFFFFFFFF)
            {
                if(LTime > Meas.Time)
                {
                  LTime = Meas.Time;
                  SaveI = i;
                }
            }
        }

        for (int i = SaveI; i < JourSize; i+= RecordSize)
        {
            mem = static_cast<char *>(File->at(0).thedata);
            mem +=i;
            memcpy(&Meas, mem, tmpt);

            if(Meas.Time != 0xFFFFFFFF)
            {
                EventNum << Meas.NUM;
                Time << TimeFunc::UnixTime64ToString(Meas.Time);
                UeffA << Meas.Ueff[0];
                UeffB << Meas.Ueff[1];
                UeffC << Meas.Ueff[2];
                IeffA << Meas.Ieff[0];
                IeffB << Meas.Ieff[1];
                IeffC << Meas.Ieff[2];
                U0 << Meas.U0;
                U1 << Meas.U1;
                U2 << Meas.U2;
                I0 << Meas.I0;
                I1 << Meas.I1;
                I2 << Meas.I2;
                CbushA << Meas.Cbush[0];
                CbushB << Meas.Cbush[1];
                CbushC << Meas.Cbush[2];
                Tg_dA << Meas.Tg_d[0];
                Tg_dB << Meas.Tg_d[1];
                Tg_dC << Meas.Tg_d[2];
                dCbushA << Meas.dCbush[0];
                dCbushB << Meas.dCbush[1];
                dCbushC << Meas.dCbush[2];
                dTg_dA << Meas.dTg_d[0];
                dTg_dB << Meas.dTg_d[1];
                dTg_dC << Meas.dTg_d[2];
                Iunb << Meas.Iunb;
                Phy_unb << Meas.Phy_unb;
                Tmk << Meas.Tmk;
                Tokr << Meas.Tamb;


                //++counter;
                //EventNum << counter;
            }

        }

        if(SaveI != 0)
        {
            for (int i = 0; i < (SaveI); i+= RecordSize)
            {
                mem = static_cast<char *>(File->at(0).thedata);
                mem +=i;
                memcpy(&Meas, mem, tmpt);

                if(Meas.Time != 0xFFFFFFFF)
                {
                    EventNum << Meas.NUM;
                    Time << TimeFunc::UnixTime64ToString(Meas.Time);
                    UeffA << Meas.Ueff[0];
                    UeffB << Meas.Ueff[1];
                    UeffC << Meas.Ueff[2];
                    IeffA << Meas.Ieff[0];
                    IeffB << Meas.Ieff[1];
                    IeffC << Meas.Ieff[2];
                    U0 << Meas.U0;
                    U1 << Meas.U1;
                    U2 << Meas.U2;
                    I0 << Meas.I0;
                    I1 << Meas.I1;
                    I2 << Meas.I2;
                    CbushA << Meas.Cbush[0];
                    CbushB << Meas.Cbush[1];
                    CbushC << Meas.Cbush[2];
                    Tg_dA << Meas.Tg_d[0];
                    Tg_dB << Meas.Tg_d[1];
                    Tg_dC << Meas.Tg_d[2];
                    dCbushA << Meas.dCbush[0];
                    dCbushB << Meas.dCbush[1];
                    dCbushC << Meas.dCbush[2];
                    dTg_dA << Meas.dTg_d[0];
                    dTg_dB << Meas.dTg_d[1];
                    dTg_dC << Meas.dTg_d[2];
                    Iunb << Meas.Iunb;
                    Phy_unb << Meas.Phy_unb;
                    Tmk << Meas.Tmk;
                    Tokr << Meas.Tamb;
                }

            }
        }

        lsl.append(EventNum);
        lsl.append(Time);
        lsl.append(UeffA);
        lsl.append(UeffB);
        lsl.append(UeffC);
        lsl.append(IeffA);
        lsl.append(IeffB);
        lsl.append(IeffC);
        lsl.append(U0);
        lsl.append(U1);
        lsl.append(U2);
        lsl.append(I0);
        lsl.append(I1);
        lsl.append(I2);
        lsl.append(CbushA);
        lsl.append(CbushB);
        lsl.append(CbushC);
        lsl.append(Tg_dA);
        lsl.append(Tg_dB);
        lsl.append(Tg_dC);
        lsl.append(dCbushA);
        lsl.append(dCbushB);
        lsl.append(dCbushC);
        lsl.append(dTg_dA);
        lsl.append(dTg_dB);
        lsl.append(dTg_dC);
        lsl.append(Iunb);
        lsl.append(Phy_unb);
        lsl.append(Tmk);
        lsl.append(Tokr);

        TableMeasModel->fillModel(lsl);
        UpdateMeasModel();
        QApplication::restoreOverrideCursor();

}

void JournalDialog::UpdateSysModel()
{
    ETableView *tv = this->findChild<ETableView *>("system");
    if (tv == nullptr)
    {
        DBGMSG;
        return; // !!! системная проблема
    }
    QItemSelectionModel *m = tv->selectionModel();
    tv->setModel(TableSysModel);
    delete m;
    //GetOscPBDelegate *dg = new GetOscPBDelegate;
    //connect(dg,SIGNAL(clicked(QModelIndex)),this,SLOT(GetOsc(QModelIndex)));
    //tv->setItemDelegateForColumn(4, dg); // устанавливаем делегата (кнопки "Скачать") для соотв. столбца
    //tv->resizeRowsToContents();
    tv->resizeColumnsToContents();
}

void JournalDialog::UpdateWorkModel()
{
    ETableView *tv = this->findChild<ETableView *>("work");
    if (tv == nullptr)
    {
        DBGMSG;
        return; // !!! системная проблема
    }
    QItemSelectionModel *m = tv->selectionModel();
    tv->setModel(TableWorkModel);
    delete m;
    //GetOscPBDelegate *dg = new GetOscPBDelegate;
    //connect(dg,SIGNAL(clicked(QModelIndex)),this,SLOT(GetOsc(QModelIndex)));
    //tv->setItemDelegateForColumn(4, dg); // устанавливаем делегата (кнопки "Скачать") для соотв. столбца
    //tv->resizeRowsToContents();
    tv->resizeColumnsToContents();
}

void JournalDialog::UpdateMeasModel()
{
    ETableView *tv = this->findChild<ETableView *>("meas");
    if (tv == nullptr)
    {
        DBGMSG;
        return; // !!! системная проблема
    }
    QItemSelectionModel *m = tv->selectionModel();
    tv->setModel(TableMeasModel);
    delete m;
    //GetOscPBDelegate *dg = new GetOscPBDelegate;
    //connect(dg,SIGNAL(clicked(QModelIndex)),this,SLOT(GetOsc(QModelIndex)));
    //tv->setItemDelegateForColumn(4, dg); // устанавливаем делегата (кнопки "Скачать") для соотв. столбца
    tv->resizeRowsToContents();
    tv->resizeColumnsToContents();
}

void JournalDialog::SaveMeasToTXTFile()
{
    int res = Error::ER_NOERROR;
    QByteArray ba;
    if((SaveMJour != nullptr) && (SaveMJour->at(0).num_byte != 0))
    {
        ba.resize(SaveMJour->at(0).num_byte);
        memcpy(&(ba.data()[0]), &SaveMJour->data()[0], SaveMJour->at(0).num_byte);
        res = Files::SaveToFile(Files::ChooseFileForSave(this, "Journal files (*.txt)", "txt"), ba, SaveMJour->at(0).num_byte);
        switch (res)
        {
        case Files::ER_NOERROR:
            EMessageBox::information(this, "Внимание", "Журнал успешно записан в файл!");
            break;
        case Files::ER_FILEWRITE:
            EMessageBox::error(this, "Ошибка", "Ошибка при записи файла!");
            break;
        case Files::ER_FILENAMEEMP:
            EMessageBox::error(this, "Ошибка", "Пустое имя файла!");
            break;
        case Files::ER_FILEOPEN:
            EMessageBox::error(this, "Ошибка", "Ошибка открытия файла!");
            break;
        default:
            break;
        }
    }
}

void JournalDialog::SaveSysToTXTFile()
{
    int res = Error::ER_NOERROR;
    QByteArray ba;
    if((SaveSJour != nullptr) && (SaveSJour->at(0).num_byte != 0))
    {
        ba.resize(SaveSJour->size());
        memcpy(&(ba.data()[0]), &SaveSJour->data()[0], SaveSJour->at(0).num_byte);
        res = Files::SaveToFile(Files::ChooseFileForSave(this, "Journal files (*.txt)", "txt"), ba, SaveSJour->size());
        switch (res)
        {
        case Files::ER_NOERROR:
            EMessageBox::information(this, "Внимание", "Журнал успешно записан в файл!");
            break;
        case Files::ER_FILEWRITE:
            EMessageBox::error(this, "Ошибка", "Ошибка при записи файла!");
            break;
        case Files::ER_FILENAMEEMP:
            EMessageBox::error(this, "Ошибка", "Пустое имя файла!");
            break;
        case Files::ER_FILEOPEN:
            EMessageBox::error(this, "Ошибка", "Ошибка открытия файла!");
            break;
        default:
            break;
        }
    }
}

void JournalDialog::SaveWorkToTXTFile()
{
    int res = Error::ER_NOERROR;
    QByteArray ba;
    if((SaveWJour != nullptr) && (SaveWJour->at(0).num_byte != 0))
    {
        ba.resize(SaveWJour->at(0).num_byte);
        int size = sizeof(*TableWorkModel);
        memcpy(&(ba.data()[0]), (char*)TableWorkModel, sizeof(*TableWorkModel));
        res = Files::SaveToFile(Files::ChooseFileForSave(this, "Journal files (*.txt)", "txt"), ba, SaveWJour->size());
        switch(res)
        {
        case Files::ER_NOERROR:
            EMessageBox::information(this, "Внимание", "Журнал успешно записан в файл!");
            break;
        case Files::ER_FILEWRITE:
            EMessageBox::error(this, "Ошибка", "Ошибка при записи файла!");
            break;
        case Files::ER_FILENAMEEMP:
            EMessageBox::error(this, "Ошибка", "Пустое имя файла!");
            break;
        case Files::ER_FILEOPEN:
            EMessageBox::error(this, "Ошибка", "Ошибка открытия файла!");
            break;
        default:
            break;
        }
    }
}

#endif



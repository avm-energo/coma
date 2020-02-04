#include <QGroupBox>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QApplication>
#include "../widgets/emessagebox.h"
#include "../gen/maindef.h"
#include "../widgets/getoscpbdelegate.h"
#include "../widgets/wd_func.h"
#include "../gen/stdfunc.h"
#include "../gen/error.h"
#include "../gen/timefunc.h"
#include "../dialogs/journalsdialog.h"
#include "../gen/colors.h"


JournalDialog::JournalDialog() : QDialog()
{
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
    TableModel = new ETableModel;

    //int i;
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    //QTabWidget *ConfTW = new QTabWidget;
    QTabWidget *work = new QTabWidget;
    work->setObjectName("conftw1");
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
    vlyout->addLayout(hlyout);
    vlyout->addWidget(tv, 89);

    work->setLayout(vlyout);
    work->setStyleSheet(ConfTWss);

    QTabWidget *system = new QTabWidget;
    //system->setObjectName("conftw2");
    system->tabBar()->setStyleSheet(ConfTWss);

    hlyout = new QHBoxLayout;
    vlyout = new QVBoxLayout;
    tv = new ETableView;
    tv->setObjectName("conftw2");
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
    vlyout->addLayout(hlyout);
    vlyout->addWidget(tv, 89);

    system->setLayout(vlyout);
    system->setStyleSheet(ConfTWss);

    QTabWidget *measure = new QTabWidget;
    measure->setObjectName("conftw3");
    measure->tabBar()->setStyleSheet(ConfTWss);
    hlyout = new QHBoxLayout;
    vlyout = new QVBoxLayout;
    tv = new ETableView;

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
    char num = 4;

    emit ReadJour(&num);

}

void JournalDialog::GetWorkJour()
{

}

void JournalDialog::GetMeasJour()
{

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

        TableModel->ClearModel();
        TableModel->addColumn(" № ");
        TableModel->addColumn("Описание события");
        TableModel->addColumn("Дата/Время");
        TableModel->addColumn("Тип события");

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
        TableModel->fillModel(lsl);
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

        TableModel->ClearModel();
        TableModel->addColumn(" № ");
        TableModel->addColumn("Описание события");
        TableModel->addColumn("Дата/Время");
        TableModel->addColumn("Тип события");

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
                  Num << Discription.at(N);
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

        for (int i = 0; i < (JourSize - SaveI); i+= RecordSize)
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
                  Num << Discription.at(N);
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

        lsl.append(EventNum);
        lsl.append(Num);
        lsl.append(Time);
        lsl.append(Type);
        TableModel->fillModel(lsl);
        UpdateSysModel();
        QApplication::restoreOverrideCursor();

}

void JournalDialog::FillMeasJour(QVector<S2::DataRec>*)
{

}

void JournalDialog::UpdateSysModel()
{
    ETableView *tv = this->findChild<ETableView *>("conftw2");
    if (tv == nullptr)
    {
        DBGMSG;
        return; // !!! системная проблема
    }
    QItemSelectionModel *m = tv->selectionModel();
    tv->setModel(TableModel);
    delete m;
    //GetOscPBDelegate *dg = new GetOscPBDelegate;
    //connect(dg,SIGNAL(clicked(QModelIndex)),this,SLOT(GetOsc(QModelIndex)));
    //tv->setItemDelegateForColumn(4, dg); // устанавливаем делегата (кнопки "Скачать") для соотв. столбца
    tv->resizeRowsToContents();
    tv->resizeColumnsToContents();
}

#endif



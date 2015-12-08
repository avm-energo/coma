#include <QTabWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QCoreApplication>
#include "a_tunedialog.h"
#include "../publicclass.h"
#include "../canal.h"

a_tunedialog::a_tunedialog(QWidget *parent) :
    QDialog(parent)
{
    NoProperCoefs = false;
    setAttribute(Qt::WA_DeleteOnClose);
//    cn = new canal;
    for (int i = 0; i < 16; i++)
    {
        Bda0.sin[i] = 0;
        Bda5.sin[i] = UINT_MAX;
        Bda20.sin[i] = UINT_MAX;
    }
    SetupUI();
}

void a_tunedialog::SetupUI()
{
    int i;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: blue; font: bold 10px;}";
    QWidget *cp1 = new QWidget;
    QWidget *cp2 = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl;
    QVBoxLayout *gb1lyout = new QVBoxLayout;
    QHBoxLayout *gb2lyout = new QHBoxLayout;

    QTabWidget *TuneTW = new QTabWidget;
    TuneTW->addTab(cp1,"Коэффициенты");
    TuneTW->addTab(cp2,"Настройка");
    QGroupBox *gb = new QGroupBox("Настроечные коэффициенты");
    for (i = 0; i < 16; i++)
    {
        lbl = new QLabel("b"+QString::number(i)+":");
        gb2lyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("tunebcoef"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        gb2lyout->addWidget(lbl, 1);
        lbl = new QLabel("u"+QString::number(i)+":");
        gb2lyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("tunek1coef"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        gb2lyout->addWidget(lbl, 1);
        lbl = new QLabel("i"+QString::number(i)+":");
        gb2lyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("tunek2coef"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        gb2lyout->addWidget(lbl, 1);
        if ((i>0)&&!((i+1)%2))
        {
            gb1lyout->addLayout(gb2lyout);
            gb2lyout = new QHBoxLayout;
        }
    }
    if (gb2lyout->count())
        gb1lyout->addLayout(gb2lyout);
    QPushButton *pb = new QPushButton("Прочитать настроечные коэффициенты из модуля");
    connect(pb,SIGNAL(clicked()),this,SLOT(ReadTuneCoefs()));
    if (pc.Emul)
        pb->setEnabled(false);
    gb1lyout->addWidget(pb);
    pb = new QPushButton("Записать настроечные коэффициенты в модуль");
    connect(pb,SIGNAL(clicked()),this,SLOT(WriteTuneCoefs()));
    if (pc.Emul)
        pb->setEnabled(false);
    gb1lyout->addWidget(pb);
    pb = new QPushButton("Прочитать настроечные коэффициенты из файла");
    connect(pb,SIGNAL(clicked()),this,SLOT(LoadFromFile()));
    gb1lyout->addWidget(pb);
    pb = new QPushButton("Записать настроечные коэффициенты в файл");
    connect(pb,SIGNAL(clicked()),this,SLOT(SaveToFile()));
    gb1lyout->addWidget(pb);
    gb->setLayout(gb1lyout);
    lyout->addWidget(gb);
    cp1->setLayout(lyout);

    QGridLayout *glyout = new QGridLayout;
    lbl = new QLabel("Канал");
    glyout->addWidget(lbl,0,0,1,1,Qt::AlignCenter);
    lbl = new QLabel("Значение");
    glyout->addWidget(lbl,0,1,1,1,Qt::AlignCenter);
    lbl = new QLabel("Настройка нуля");
    glyout->addWidget(lbl,0,2,1,1,Qt::AlignCenter);
    lbl = new QLabel("Настройка 20 мА");
    glyout->addWidget(lbl,0,3,1,1,Qt::AlignCenter);
    lbl = new QLabel("Настройка 5 В");
    glyout->addWidget(lbl,0,4,1,1,Qt::AlignCenter);
    glyout->setColumnStretch(0, 0);
    glyout->setColumnStretch(1, 3);
    glyout->setColumnStretch(2, 1);
    glyout->setColumnStretch(3, 1);
    glyout->setColumnStretch(4, 1);
    for (i = 0; i < 16; i++)
    {
        lbl = new QLabel(QString::number(i));
        glyout->addWidget(lbl,i+1,0,1,1,Qt::AlignCenter);
        lbl = new QLabel("");
        lbl->setObjectName("tunech"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        glyout->addWidget(lbl,i+1,1,1,1);
        pb = new QPushButton("Запуск");
        pb->setObjectName("0"+QString::number(i));
        connect(pb,SIGNAL(clicked()),this,SLOT(StartTune()));
        if (pc.Emul)
            pb->setEnabled(false);
        glyout->addWidget(pb,i+1,2,1,1,Qt::AlignCenter);
        pb = new QPushButton("Запуск");
        pb->setObjectName("1"+QString::number(i));
        connect(pb,SIGNAL(clicked()),this,SLOT(StartTune()));
        if (pc.Emul)
            pb->setEnabled(false);
        glyout->addWidget(pb,i+1,3,1,1,Qt::AlignCenter);
        pb = new QPushButton("Запуск");
        pb->setObjectName("2"+QString::number(i));
        connect(pb,SIGNAL(clicked()),this,SLOT(StartTune()));
        if (pc.Emul)
            pb->setEnabled(false);
        glyout->addWidget(pb,i+1,4,1,1,Qt::AlignCenter);
    }
    cp2->setLayout(glyout);
    lyout = new QVBoxLayout;
    lyout->addWidget(TuneTW);
    setLayout(lyout);
}

bool a_tunedialog::tune(int Type, int ChNum)
{
    switch (Type)
    {
    case 0: // настройка нуля
    {
        if (QMessageBox::information(this,"Настройка",\
                                     "Переключите входные переключатели на напряжение,\nустановите напряжение 0 В " \
                                     "на всех\nвходах модуля и нажмите OK")\
                == QMessageBox::Ok)
        {
            cn->Send(CN_Gda, &Bda0, sizeof(Bda));
            while (cn->Busy)
                QCoreApplication::processEvents(QEventLoop::AllEvents);
            if (cn->result == CN_OK)
                CheckAndShowTune0(ChNum);
            else
            {
                ATUNEWARN("");
                return false;
            }
        }
        else
        {
            ATUNEWARN("Настройка прервана");
            return false;
        }
        break;
    }
    case 1: // настройка 20 мА
    {
        if (QMessageBox::information(this,"Настройка",\
                                     "Переключите входные переключатели на ток,\nустановите ток 20 мА на всех" \
                                     "\nвходах модуля и нажмите OK") == QMessageBox::Ok)
        {
            cn->Send(CN_Gda, &Bda20, sizeof(Bda));
            while (cn->Busy)
                QCoreApplication::processEvents(QEventLoop::AllEvents);
            if (cn->result == CN_OK)
                CheckAndShowTune20(ChNum);
            else
            {
                ATUNEWARN("");
                return false;
            }
        }
        else
        {
            ATUNEWARN("Настройка прервана");
            return false;
        }
        break;
    }
    case 2: // настройка 5 В
    {
        if (QMessageBox::information(this,"Настройка",\
                                     "Переключите входные переключатели на напряжение,\nустановите напряжение" \
                                     "5 В на всех\nвходах модуля и нажмите OK")\
                == QMessageBox::Ok)
        {
            cn->Send(CN_Gda, &Bda5, sizeof(Bda));
            while (cn->Busy)
                QCoreApplication::processEvents(QEventLoop::AllEvents);
            if (cn->result == CN_OK)
                CheckAndShowTune5(ChNum);
            else
            {
                ATUNEWARN("");
                return false;
            }
        }
        else
        {
            ATUNEWARN("Настройка прервана");
            return false;
        }
        break;
    }
    default:
        return false;
        break;
    }
    return true;
}

void a_tunedialog::StartTune()
{
    QString AllNum = sender()->objectName();
    if (!AllNum.isEmpty())
    {
        int Type = AllNum.at(0).digitValue();
        if (Type != -1)
        {
            int ChNum = AllNum.right(AllNum.size()-1).toInt();
            if (ChNum != -1)
            {
                QPushButton *pb = qobject_cast<QPushButton *>(sender());
                if (tune(Type, ChNum))
                {
                    if (pb != 0)
                        pb->setStyleSheet("QPushButton {background-color: #FFE0C0;}");
                    ATUNEINFO("Успешно");
                }
                else
                {
                    if (pb != 0)
                        pb->setStyleSheet("QPushButton {background-color: #D0D0D0;}");
                    ATUNEWARN("Настройка не удалась");
                }
            }
            else
                ATUNEDBG;
        }
        else
            ATUNEDBG;
    }
    else
        ATUNEDBG;
}

bool a_tunedialog::CheckAndShowTune0(int ChNum)
{
    QLabel *lbl = this->findChild<QLabel *>("tunech"+QString::number(ChNum));
    if (lbl == 0)
    {
        ATUNEDBG;
        return false;
    }
    lbl->setText(QString::number(Bda0.sin[ChNum]/ATUNENUMPOINTS,16));
    if (!CalcNewTuneCoef(ChNum))
        return false;
    if (!RefreshTuneCoef(ChNum))
        return false;
    return true;
}

bool a_tunedialog::CheckAndShowTune5(int ChNum)
{
    QLabel *lbl = this->findChild<QLabel *>("tunech"+QString::number(ChNum));
    if (lbl == 0)
    {
        ATUNEDBG;
        return false;
    }
    lbl->setText(QString::number(Bda5.sin[ChNum]/ATUNENUMPOINTS,16));
    if (!CalcNewTuneCoef(ChNum))
        return false;
    if (!RefreshTuneCoef(ChNum))
        return false;
    return true;
}

bool a_tunedialog::CheckAndShowTune20(int ChNum)
{
    QLabel *lbl = this->findChild<QLabel *>("tunech"+QString::number(ChNum));
    if (lbl == 0)
    {
        ATUNEDBG;
        return false;
    }
    lbl->setText(QString::number(Bda20.sin[ChNum]/ATUNENUMPOINTS,16));
    if (!CalcNewTuneCoef(ChNum))
        return false;
    if (!RefreshTuneCoef(ChNum))
        return false;
    return true;
}

bool a_tunedialog::CalcNewTuneCoef(int ChNum)
{
    Bac_block[ChNum].fbin = 1.25 - (static_cast<float>(Bda0.sin[ChNum]) / \
                                        (ATUNENUMPOINTS*1638.0));
    if ((Bda0.sin[ChNum] == Bda5.sin[ChNum]) || (Bda0.sin[ChNum] == Bda20.sin[ChNum]))
    {
        ATUNEWARN("Ошибка в настроечных коэффициентах, деление на ноль");
        return false;
    }
    Bac_block[ChNum].fkuin = ATUNENUMPOINTS*1638.0 / \
            static_cast<float>(Bda0.sin[ChNum]-Bda5.sin[ChNum]);
    Bac_block[ChNum].fkiin = ATUNENUMPOINTS*1638.0 / \
            static_cast<float>(Bda0.sin[ChNum]-Bda20.sin[ChNum]);
    return true;
}

bool a_tunedialog::RefreshTuneCoef(int ChNum)
{
    QLabel *lbl = this->findChild<QLabel *>("tunebcoef"+QString::number(ChNum));
    if (lbl == 0)
    {
        ATUNEDBG;
        return false;
    }
    lbl->setText(QString::number(Bac_block[ChNum].fbin, 'f', 5));
    lbl = this->findChild<QLabel *>("tunek1coef"+QString::number(ChNum));
    if (lbl == 0)
    {
        ATUNEDBG;
        return false;
    }
    lbl->setText(QString("%1").arg(Bac_block[ChNum].fkuin, 0, 'f', 5));
    lbl = this->findChild<QLabel *>("tunek2coef"+QString::number(ChNum));
    if (lbl == 0)
    {
        ATUNEDBG;
        return false;
    }
    lbl->setText(QString::number(Bac_block[ChNum].fkiin, 'f', 5));
    return true;
}

void a_tunedialog::RefreshTuneCoefs()
{
    for (int i=0; i<16; i++)
        RefreshTuneCoef(i);
}

void a_tunedialog::ReadTuneCoefs()
{
    cn->Send(CN_Gac, &Bac_block, sizeof(Bac_block));
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        RefreshTuneCoefs();
}

void a_tunedialog::WriteTuneCoefs()
{
    if (CheckTuneCoefs())
    {
        cn->Send(CN_Wac, &Bac_block, sizeof(Bac_block));
        while (cn->Busy)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
        if (cn->result == CN_OK)
            QMessageBox::information(this,"Успешно!","Записано успешно!");
        else
            return;
    }
    else
    {
        ATUNEWARN("Есть некорректные коэффициенты");
        return;
    }
}

bool a_tunedialog::CheckTuneCoefs()
{
    for (int i=0; i<16; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>("tunebcoef"+QString::number(i));
        if (lbl == 0)
        {
            ATUNEDBG;
            return false;
        }
        if (lbl->text().isEmpty() || (lbl->text().toInt() == -1))
            return false;
        lbl = this->findChild<QLabel *>("tunek1coef"+QString::number(i));
        if (lbl == 0)
        {
            ATUNEDBG;
            return false;
        }
        if (lbl->text().isEmpty() || (lbl->text().toInt() == -1))
            return false;
        lbl = this->findChild<QLabel *>("tunek2coef"+QString::number(i));
        if (lbl == 0)
        {
            ATUNEDBG;
            return false;
        }
        if (lbl->text().isEmpty() || (lbl->text().toInt() == -1))
            return false;
    }
    return true;
}

void a_tunedialog::LoadFromFile()
{
    int res = pc.LoadFile("Tune files (*.atn)", sizeof(Bac_block));
    switch (res)
    {
    case 0:
        break;
    case 1:
        ATUNEER("Ошибка открытия файла!");
        return;
        break;
    case 2:
        if (QMessageBox::question(this,"Не тот файл","В файле содержатся данные для модуля с другим CPUID и/или SN.\nПродолжить загрузку?",\
                                  QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Ok);
        else
            return;
        break;
    case 3:
        ATUNEER("Пустое имя файла!");
        return;
        break;
    case 4:
        ATUNEER("Ошибка открытия файла!");
        return;
        break;
    default:
        return;
        break;
    }
    pc.LoadFileToPtr(&Bac_block, sizeof(Bac_block));
    RefreshTuneCoefs();
    ATUNEINFO("Загрузка прошла успешно!");
}

void a_tunedialog::SaveToFile()
{
    int res = pc.SaveFile("Tune files (*.atn)", &Bac_block, sizeof(Bac_block));
    switch (res)
    {
    case 0:
        ATUNEINFO("Записано успешно!");
        break;
    case 1:
        ATUNEER("Ошибка при записи файла!");
        break;
    case 2:
        ATUNEER("Пустое имя файла!");
        break;
    case 3:
        ATUNEER("Ошибка открытия файла!");
        break;
    default:
        break;
    }
}

void a_tunedialog::ShowErrMsg(int ermsg)
{
    if (ermsg < pc.errmsgs.size())
        ATUNEER(pc.errmsgs.at(ermsg));
    else
        ATUNEER("Произошла неведомая фигня #"+QString::number(ermsg,10));}

void a_tunedialog::UpdateProper(bool tmpb)
{
    NoProperCoefs = tmpb;
}

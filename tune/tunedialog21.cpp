#include <QTabWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QCoreApplication>
#include "tunedialog21.h"
#include "../widgets/emessagebox.h"
#include "../gen/commands.h"

TuneDialog21::TuneDialog21(int type, QWidget *parent) :
    EAbstractTuneDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    for (int i = 0; i < 16; i++)
    {
        Bda0.sin[i] = 0;
        Bda5.sin[i] = UINT_MAX;
        Bda20.sin[i] = UINT_MAX;
    }
    BoardType = type;
    SetupUI();
}

void TuneDialog21::SetupUI()
{
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    int i;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QString ValuesLEFormat = "QLineEdit {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *cp1 = new QWidget;
    QWidget *cp2 = new QWidget;
    tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp1->setStyleSheet(tmps);
    cp2->setStyleSheet(tmps);
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
        QLineEdit *le = new QLineEdit("");
        le->setObjectName("tunebcoef"+QString::number(i));
        le->setStyleSheet(ValuesLEFormat);
        gb2lyout->addWidget(le, 1);
        lbl = new QLabel("u"+QString::number(i)+":");
        gb2lyout->addWidget(lbl);
        le = new QLineEdit("");
        le->setObjectName("tunek1coef"+QString::number(i));
        le->setStyleSheet(ValuesLEFormat);
        gb2lyout->addWidget(le, 1);
        lbl = new QLabel("i"+QString::number(i)+":");
        gb2lyout->addWidget(lbl);
        le = new QLineEdit("");
        le->setObjectName("tunek2coef"+QString::number(i));
        le->setStyleSheet(ValuesLEFormat);
        gb2lyout->addWidget(le, 1);
        if ((i>0)&&!((i+1)%2))
        {
            gb1lyout->addLayout(gb2lyout);
            gb2lyout = new QHBoxLayout;
        }
    }
    if (gb2lyout->count())
        gb1lyout->addLayout(gb2lyout);
    QPushButton *pb = new QPushButton("Установить настроечные коэффициенты по умолчанию");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetDefCoefs()));
    gb1lyout->addWidget(pb);
    pb = new QPushButton("Прочитать настроечные коэффициенты из модуля");
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
    pb->setIcon(QIcon("images/load.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(LoadFromFile()));
    gb1lyout->addWidget(pb);
    pb = new QPushButton("Записать настроечные коэффициенты в файл");
    pb->setIcon(QIcon("images/save.png"));
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
    if ((!(pc.ModuleBsi.Hth & HTH_REGPARS)) && !pc.Emul) // есть настроечные коэффициенты в памяти модуля
        ReadTuneCoefs(); // считать их из модуля и показать на экране
}

void TuneDialog21::SetLbls()
{

}

void TuneDialog21::SetPf()
{

}

void TuneDialog21::FillBac()
{

}

void TuneDialog21::FillBackBac()
{

}

void TuneDialog21::GetBdAndFillMTT()
{

}

bool TuneDialog21::tune(int Type, int ChNum)
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
            if (Commands::GetBda(BT_NONE, &Bda0, sizeof(Check21::Bda)) == NOERROR)
                CheckAndShowTune0(ChNum);
            else
            {
                WARNMSG("");
                return false;
            }
        }
        else
        {
            WARNMSG("Настройка прервана");
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
            if (Commands::GetBda(BT_NONE, &Bda20, sizeof(Check21::Bda)) == NOERROR)
                CheckAndShowTune20(ChNum);
            else
            {
                WARNMSG("");
                return false;
            }
        }
        else
        {
            WARNMSG("Настройка прервана");
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
            if (Commands::GetBda(BT_NONE, &Bda5, sizeof(Check21::Bda)) == NOERROR)
                CheckAndShowTune5(ChNum);
            else
            {
                WARNMSG("");
                return false;
            }
        }
        else
        {
            WARNMSG("Настройка прервана");
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

/*void TuneDialog21::StartTune()
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
                    EMessageBox::information(this, "Внимание", "Успешно");
                }
                else
                {
                    if (pb != 0)
                        pb->setStyleSheet("QPushButton {background-color: #D0D0D0;}");
                    WARNMSG("Настройка не удалась");
                }
            }
            else
                DBGMSG;
        }
        else
            DBGMSG;
    }
    else
        DBGMSG;
} */

bool TuneDialog21::CheckAndShowTune0(int ChNum)
{
    QLabel *lbl = this->findChild<QLabel *>("tunech"+QString::number(ChNum));
    if (lbl == 0)
    {
        DBGMSG;
        return false;
    }
    lbl->setText(QString::number(Bda0.sin[ChNum]/ATUNENUMPOINTS,16));
    if (!CalcNewTuneCoef(ChNum))
        return false;
    if (!RefreshTuneField(ChNum))
        return false;
    return true;
}

bool TuneDialog21::CheckAndShowTune5(int ChNum)
{
    QLabel *lbl = this->findChild<QLabel *>("tunech"+QString::number(ChNum));
    if (lbl == 0)
    {
        DBGMSG;
        return false;
    }
    lbl->setText(QString::number(Bda5.sin[ChNum]/ATUNENUMPOINTS,16));
    if (!CalcNewTuneCoef(ChNum))
        return false;
    if (!RefreshTuneField(ChNum))
        return false;
    return true;
}

bool TuneDialog21::CheckAndShowTune20(int ChNum)
{
    QLabel *lbl = this->findChild<QLabel *>("tunech"+QString::number(ChNum));
    if (lbl == 0)
    {
        DBGMSG;
        return false;
    }
    lbl->setText(QString::number(Bda20.sin[ChNum]/ATUNENUMPOINTS,16));
    if (!CalcNewTuneCoef(ChNum))
        return false;
    if (!RefreshTuneField(ChNum))
        return false;
    return true;
}

bool TuneDialog21::CalcNewTuneCoef(int ChNum)
{
    Bac_block[ChNum].fbin = 1.25 - (static_cast<float>(Bda0.sin[ChNum]) / \
                                        (ATUNENUMPOINTS*1638.0));
    if ((Bda0.sin[ChNum] == Bda5.sin[ChNum]) || (Bda0.sin[ChNum] == Bda20.sin[ChNum]))
    {
        WARNMSG("Ошибка в настроечных коэффициентах, деление на ноль");
        return false;
    }
    Bac_block[ChNum].fkuin = ATUNENUMPOINTS*1638.0 / \
            static_cast<float>(Bda0.sin[ChNum]-Bda5.sin[ChNum]);
    Bac_block[ChNum].fkiin = ATUNENUMPOINTS*1638.0 / \
            static_cast<float>(Bda0.sin[ChNum]-Bda20.sin[ChNum]);
    return true;
}

void TuneDialog21::SetDefCoefs()
{
    for (int i=0; i<16; i++)
    {
        Bac_block[i].fbin = 0.0;
        Bac_block[i].fkiin = 1.0;
        Bac_block[i].fkuin = 1.0;
    }
    RefreshTuneFields();
}

int TuneDialog21::ReadAnalogMeasurements()
{
    return NOERROR;
}

bool TuneDialog21::RefreshTuneField(int ChNum)
{
    QLineEdit *le = this->findChild<QLineEdit *>("tunebcoef"+QString::number(ChNum));
    if (le == 0)
    {
        DBGMSG;
        return false;
    }
    le->setText(QString::number(Bac_block[ChNum].fbin, 'f', 5));
    le = this->findChild<QLineEdit *>("tunek1coef"+QString::number(ChNum));
    if (le == 0)
    {
        DBGMSG;
        return false;
    }
    le->setText(QString("%1").arg(Bac_block[ChNum].fkuin, 0, 'f', 5));
    le = this->findChild<QLineEdit *>("tunek2coef"+QString::number(ChNum));
    if (le == 0)
    {
        DBGMSG;
        return false;
    }
    le->setText(QString::number(Bac_block[ChNum].fkiin, 'f', 5));
    return true;
}

void TuneDialog21::RefreshTuneFields()
{
    for (int i=0; i<16; i++)
        RefreshTuneField(i);
}

bool TuneDialog21::RefreshTuneCoef(int ChNum)
{
    QLineEdit *le = this->findChild<QLineEdit *>("tunebcoef"+QString::number(ChNum));
    if (le == 0)
    {
        DBGMSG;
        return false;
    }
    bool ok;
    Bac_block[ChNum].fbin = le->text().toFloat(&ok);
    if (!ok)
    {
        WARNMSG("Ошибка при переводе во float");
        return false;
    }
    le = this->findChild<QLineEdit *>("tunek1coef"+QString::number(ChNum));
    if (le == 0)
    {
        DBGMSG;
        return false;
    }
    Bac_block[ChNum].fkiin = le->text().toFloat(&ok);
    if (!ok)
    {
        WARNMSG("Ошибка при переводе во float");
        return false;
    }
    le = this->findChild<QLineEdit *>("tunek2coef"+QString::number(ChNum));
    if (le == 0)
    {
        DBGMSG;
        return false;
    }
    Bac_block[ChNum].fkuin = le->text().toFloat(&ok);
    if (!ok)
    {
        WARNMSG("Ошибка при переводе во float");
        return false;
    }
    return true;
}

void TuneDialog21::RefreshTuneCoefs()
{
    for (int i=0; i<16; i++)
        RefreshTuneCoef(i);
}

bool TuneDialog21::CheckTuneCoefs()
{
    for (int i=0; i<16; i++)
    {
        QLineEdit *le = this->findChild<QLineEdit *>("tunebcoef"+QString::number(i));
        if (le == 0)
        {
            DBGMSG;
            return false;
        }
        if (le->text().isEmpty() || (le->text().toInt() == -1))
            return false;
        le = this->findChild<QLineEdit *>("tunek1coef"+QString::number(i));
        if (le == 0)
        {
            DBGMSG;
            return false;
        }
        if (le->text().isEmpty() || (le->text().toInt() == -1))
            return false;
        le = this->findChild<QLineEdit *>("tunek2coef"+QString::number(i));
        if (le == 0)
        {
            DBGMSG;
            return false;
        }
        if (le->text().isEmpty() || (le->text().toInt() == -1))
            return false;
    }
    return true;
}

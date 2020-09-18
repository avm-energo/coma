#include "cordialogktf.h"

#include "../dialogs/keypressdialog.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/s2.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../usb/commands.h"
#include "../widgets/etableview.h"
#include "../widgets/wd_func.h"

#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QStringListModel>
#include <QTabBar>
#include <QTabWidget>
#include <QVBoxLayout>

CorDialogKTF::CorDialogKTF(QWidget *parent) : AbstractCorDialog(parent)
{
    // int i;

    Bd9Block = new Bd9;
    WBd7Block = new WBd7;

    first = 0;

    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

CorDialogKTF::~CorDialogKTF()
{
}

void CorDialogKTF::SetupUI()
{
    // QWidget *cp2 = new QWidget;
    QString tmps = "QDialog {background-color: " + QString(Colors::ACONFCLR) + ";}";
    setStyleSheet(tmps);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    ETableView *tv = new ETableView;
    tv->setObjectName("cor");
    int row = 0;
    QString paramcolor = Colors::MAINWINCLR;

    glyout->addWidget(WDFunc::NewLBL(this, "Текущий расход ресурса изоляции, час:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, QString::number(907), 0, 1000000, 5, paramcolor), row, 2, 1, 2);
    row++;

    QPushButton *pb = new QPushButton("Прочитать из модуля");
    connect(pb, SIGNAL(clicked()), this, SLOT(GetCorBdButton()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, row, 1, 1, 2);

    pb = new QPushButton("Записать в модуль");
    connect(pb, SIGNAL(clicked()), this, SLOT(WriteCorBd()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, row, 3, 1, 2);

    row++;

    pb = new QPushButton("Прочитать значения из файла");
    connect(pb, SIGNAL(clicked()), this, SLOT(ReadFromFile()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, row, 1, 1, 2);

    pb = new QPushButton("Сохранить значения в файл");
    connect(pb, SIGNAL(clicked()), this, SLOT(SaveToFile()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, row, 3, 1, 2);

    lyout->addLayout(glyout, Qt::AlignTop);
    lyout->addWidget(tv, 89);
    setLayout(lyout);
}

void CorDialogKTF::FillBackCor()
{
    WDFunc::SPBData(this, QString::number(907), WBd7Block->InitAge);
}

void CorDialogKTF::FillCor()
{
    WDFunc::SetSPBData(this, QString::number(907), Bd9Block->Age);
}

void CorDialogKTF::GetCorBd(int index)
{
    if (index == corDIndex)

        switch (Board::GetInstance()->interfaceType())
        {
        case Board::InterfaceType::USB:

            // if (MainInterface == I_USB)
            {
                if (Commands::GetBd(9, Bd9Block, sizeof(Bd9)) == Error::Msg::NoError)
                {
                    FillCor();
                    QMessageBox::information(this, "INFO", "Прочитано успешно");
                }
                break;
            }
        case Board::InterfaceType::Ethernet:
            // else if (MainInterface == I_ETHERNET)
            {
                emit CorReadRequest();
                break;
            }
        }
}
void CorDialogKTF::GetCorBdButton()
{
    switch (Board::GetInstance()->interfaceType())
    {
    case Board::InterfaceType::USB:
        // if (MainInterface == I_USB)
        {
            if (Commands::GetBd(9, Bd9Block, sizeof(Bd9)) == Error::Msg::NoError)
            {
                FillCor();
                QMessageBox::information(this, "INFO", "Прочитано успешно");
            }
            break;
        }
    case Board::InterfaceType::RS485:
        // else if (MainInterface == I_RS485)
        {
            ModBus::Information info;
            info.size = (sizeof(Bd9) / 4);
            info.adr = 4000;
            emit RS485ReadCorBd(info);
            break;
        }
    case Board::InterfaceType::Ethernet:
        // else if (MainInterface == I_ETHERNET)
        {
            emit CorReadRequest();
            break;
        }
    }
}

void CorDialogKTF::WriteCorBd()
{
    // int i;
    quint32 adr = 907;

    FillBackCor();

    if (WriteCheckPassword() == Error::Msg::NoError)
    {
        switch (Board::GetInstance()->interfaceType())
        {
        case Board::InterfaceType::Ethernet:
            // if (MainInterface == I_ETHERNET)
            {

                float corblocki;
                memcpy(&corblocki, reinterpret_cast<float *>(WBd7Block), sizeof(float));
                emit SendCom50(adr, corblocki);
                TimeFunc::Wait(300);
                break;
            }
        case Board::InterfaceType::RS485:
            // else if (MainInterface == I_RS485)
            {
                ModBus::Information info;
                info.size = (sizeof(WBd7) / 4);
                info.adr = adr;
                emit RS485WriteCorBd(info, (float *)WBd7Block);
                break;
            }
        case Board::InterfaceType::USB:
            // else if (MainInterface == I_USB)
            {
                if (Commands::WriteBd(7, WBd7Block, sizeof(WBd7)) == Error::Msg::NoError)
                    QMessageBox::information(this, "INFO", "Записано успешно");
                else
                    QMessageBox::information(this, "INFO", "Ошибка");

                //......
                //            QThread::sleep(1);
                if (Commands::GetBd(9, Bd9Block, sizeof(Bd9Block)) == Error::Msg::NoError)
                    FillCor();
                break;
            }
        }
    }
}

void CorDialogKTF::WriteCor()
{
}

void CorDialogKTF::ResetCor()
{
}

void CorDialogKTF::SaveToFile()
{
    QByteArray ba;
    FillBackCor();
    ba.resize(sizeof(*WBd7Block));
    memcpy(&(ba.data()[0]), WBd7Block, sizeof(*WBd7Block));
    Error::Msg res
        = Files::SaveToFile(Files::ChooseFileForSave(this, "Tune files (*.cor)", "cor"), ba, sizeof(*WBd7Block));
    switch (res)
    {
    case Files::ER_NOERROR:
        QMessageBox::information(this, "Внимание", "Файл коэффициентов коррекции записан успешно!");
        break;
    case Files::ER_FILEWRITE:
        QMessageBox::critical(this, "Ошибка", "Ошибка при записи файла!");
        break;
    case Files::ER_FILENAMEEMP:
        QMessageBox::critical(this, "Ошибка", "Пустое имя файла!");
        break;
    case Files::ER_FILEOPEN:
        QMessageBox::critical(this, "Ошибка", "Ошибка открытия файла!");
        break;
    default:
        break;
    }
}

void CorDialogKTF::ReadFromFile()
{
    QByteArray ba;
    ba.resize(sizeof(*Bd9Block));

    Error::Msg res = Files::LoadFromFile(Files::ChooseFileForOpen(this, "Tune files (*.cor)"), ba);
    if (res != Error::Msg::NoError)
    {
        QMessageBox::critical(this, "Ошибка", "Ошибка при загрузке файла");
        ERMSG("Ошибка при загрузке файла");
        return;
    }

    //  FillCor();
    //    memcpy(Bd9Block, &(ba.data()[0]), sizeof(*Bd9Block));
    memcpy(&Bd9Block->Age, &(ba.data()[0]), sizeof(float));
    FillCor();
    QMessageBox::information(this, "Внимание", "Загрузка прошла успешно!");
}

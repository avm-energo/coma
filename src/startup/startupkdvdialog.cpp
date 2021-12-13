#include "startupkdvdialog.h"

#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
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
StartupKDVDialog::StartupKDVDialog(QWidget *parent) : AbstractStartupDialog(parent)
{
    // int i;

    Bd9Block = new Bd9;
    WBd7Block = new WBd7;
    WBd8Block = new WBd8;

    // SetupUI();
}

StartupKDVDialog::~StartupKDVDialog()
{
}

void StartupKDVDialog::SetupUI()
{

    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    ETableView *tv = new ETableView;
    tv->setObjectName("cor");
    int row = 0;

    glyout->addWidget(WDFunc::NewLBL2(this, "Текущий расход ресурса изоляции, час:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB2(this, QString::number(907), 0, 1000000, 5), row, 2, 1, 2);
    row++;
    glyout->addWidget(WDFunc::NewLBL2(this, "Текущея наработка при нормальной нагрузке, час:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB2(this, QString::number(908), 0, 1000000, 5), row, 2, 1, 2);
    row++;
    glyout->addWidget(WDFunc::NewLBL2(this, "Текущея наработка в режиме перегрузки, час:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB2(this, QString::number(909), 0, 1000000, 5), row, 2, 1, 2);
    row++;

    QPushButton *pb = new QPushButton("Прочитать из модуля");
    // NOTE Use button widget instead
    // connect(pb, &QAbstractButton::clicked, this, &AbstractStartupDialog::GetCorBd);

    glyout->addWidget(pb, row, 1, 1, 2);

    pb = new QPushButton("Записать в модуль");
    connect(pb, &QAbstractButton::clicked, this, &StartupKDVDialog::WriteCor);

    glyout->addWidget(pb, row, 3, 1, 2);

    row++;

    pb = new QPushButton("Прочитать значения из файла");
    connect(pb, &QAbstractButton::clicked, this, &StartupKDVDialog::ReadFromFile);

    glyout->addWidget(pb, row, 1, 1, 2);

    pb = new QPushButton("Сохранить значения в файл");
    connect(pb, &QAbstractButton::clicked, this, &StartupKDVDialog::SaveToFile);

    glyout->addWidget(pb, row, 3, 1, 2);

    lyout->addLayout(glyout, Qt::AlignTop);
    lyout->addWidget(tv, 89);
    setLayout(lyout);
}

void StartupKDVDialog::FillBackCor()
{
    WDFunc::SPBData(this, QString::number(907), WBd7Block->InitAge);
}

void StartupKDVDialog::FillBackWBd8()
{
    WDFunc::SPBData(this, QString::number(908), WBd8Block->MHnInit);
    WDFunc::SPBData(this, QString::number(909), WBd8Block->MotHovInit);
}

void StartupKDVDialog::FillCor()
{
    WDFunc::SetSPBData(this, QString::number(907), Bd9Block->Age);
    WDFunc::SetSPBData(this, QString::number(908), Bd9Block->MotHnorm);
    WDFunc::SetSPBData(this, QString::number(909), Bd9Block->MotHover);
}

void StartupKDVDialog::GetCorBd()
{
    //    if (index == corDIndex)

    switch (Board::GetInstance().interfaceType())
    {
        //    case Board::InterfaceType::USB:

        //        // if (MainInterface == I_USB)
        //        {
        //            if (Commands::GetBd(9, Bd9Block, sizeof(Bd9)) == Error::Msg::NoError)
        //            {
        //                FillCor();
        //                QMessageBox::information(this, "INFO", "Прочитано успешно");
        //            }
        //            break;
        //        }
        //    case Board::InterfaceType::Ethernet:
        //        // else if (MainInterface == I_ETHERNET)
        //        {
        //            emit CorReadRequest();
        //            break;
        //        }
    }
}
// void StartupKDVDialog::GetCorBdButton()
//{
//    switch (Board::GetInstance().interfaceType())
//    {
//    case Board::InterfaceType::USB:
//    {
//        if (Commands::GetBd(9, Bd9Block, sizeof(Bd9)) == Error::Msg::NoError)
//        {
//            FillCor();
//            QMessageBox::information(this, "INFO", "Прочитано успешно");
//        }
//        break;
//    }
//    case Board::InterfaceType::RS485:
//    {
//        ModBus::Information info;
//        info.size = (sizeof(Bd9) / 4);
//        info.adr = 4000;
//        emit RS485ReadCorBd(info);
//        break;
//    }
//    case Board::InterfaceType::Ethernet:
//    {
//        emit CorReadRequest();
//        break;
//    }
//    }
//}

void StartupKDVDialog::WriteCor()
{
    int i;
    quint32 adr7bl = 907;
    quint32 adr[2] = { 908, 909 };

    FillBackCor();
    FillBackWBd8();

    if (checkPassword())
    {
        switch (Board::GetInstance().interfaceType())
        {
            //        case Board::InterfaceType::Ethernet:
            //        {
            //            float corblocki;
            //            memcpy(&corblocki, reinterpret_cast<float *>(WBd7Block), sizeof(float));
            //            emit SendCom50(adr7bl, corblocki);
            //            TimeFunc::Wait(300);

            //            for (i = 0; i < 2; i++)
            //            {
            //                float corblocki;
            //                memcpy(&corblocki, reinterpret_cast<float *>(WBd8Block) + i, sizeof(float));
            //                emit SendCom50(adr[i], corblocki);
            //                TimeFunc::Wait(300);
            //            }

            //            break;
            //        }
            //        case Board::InterfaceType::RS485:
            //            // else if (MainInterface == I_RS485)
            //            {
            //                ModBus::Information info;
            //                info.size = (sizeof(WBd7) / 4);
            //                info.adr = adr7bl;
            //                emit RS485WriteCorBd(info, (float *)WBd7Block);

            //                ModBus::Information inform;
            //                inform.size = (sizeof(WBd8) / 4);
            //                inform.adr = adr[0];
            //                emit RS485WriteCorBd(inform, (float *)WBd8Block);

            //                break;
            //            }
            //        case Board::InterfaceType::USB:
            //            // else if (MainInterface == I_USB)
            //            {
            //                if ((Commands::WriteBd(7, WBd7Block, sizeof(WBd7)) == Error::Msg::NoError)
            //                    & (Commands::WriteBd(8, WBd8Block, sizeof(WBd8)) == Error::Msg::NoError))
            //                    QMessageBox::information(this, "INFO", "Записано успешно");
            //                else
            //                    QMessageBox::information(this, "INFO", "Ошибка");

            //                //......
            //                //            QThread::sleep(1);
            //                if (Commands::GetBd(9, Bd9Block, sizeof(Bd9)) == Error::Msg::NoError)
            //                    FillCor();
            //                break;
            //            }
        }
    }
}

void StartupKDVDialog::SetupCor()
{
}

void StartupKDVDialog::ResetCor()
{
}

void StartupKDVDialog::SaveToFile()
{
    QByteArray ba;
    FillBackCor();
    FillBackWBd8();
    ba.resize(sizeof(*WBd7Block) + sizeof(*WBd8Block));
    memcpy(&(ba.data()[0]), WBd7Block, sizeof(*WBd7Block));
    memcpy(&(ba.data()[sizeof(*WBd7Block)]), WBd8Block, sizeof(*WBd8Block));
    Error::Msg res = Files::SaveToFile(WDFunc::ChooseFileForSave(this, "Tune files (*.cor)", "cor"), ba);
    switch (res)
    {
    case Error::Msg::NoError:
        QMessageBox::information(this, "Внимание", "Файл коэффициентов коррекции записан успешно!");
        break;
    case Error::Msg::FileWriteError:
        QMessageBox::critical(this, "Ошибка", "Ошибка при записи файла!");
        break;
    case Error::Msg::FileNameError:
        QMessageBox::critical(this, "Ошибка", "Пустое имя файла!");
        break;
    case Error::Msg::FileOpenError:
        QMessageBox::critical(this, "Ошибка", "Ошибка открытия файла!");
        break;
    default:
        break;
    }
}

void StartupKDVDialog::ReadFromFile()
{
    QByteArray ba;
    ba.resize(sizeof(*Bd9Block));

    Error::Msg res = Files::LoadFromFile(WDFunc::ChooseFileForOpen(this, "Tune files (*.cor)"), ba);
    if (res != Error::Msg::NoError)
    {
        QMessageBox::critical(this, "Ошибка", "Ошибка при загрузке файла");
        ERMSG("Ошибка при загрузке файла");
        return;
    }

    memcpy(&Bd9Block->Age, &(ba.data()[0]), sizeof(float));
    memcpy(&Bd9Block->MotHnorm, &(ba.data()[sizeof(*WBd7Block)]), sizeof(float));
    memcpy(&Bd9Block->MotHover, &(ba.data()[sizeof(*WBd7Block) + sizeof(float)]), sizeof(float));
    FillCor();
    QMessageBox::information(this, "Внимание", "Загрузка прошла успешно!");
}

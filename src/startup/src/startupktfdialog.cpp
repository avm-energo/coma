#include "startup/startupktfdialog.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <gen/colors.h>
#include <gen/error.h>
#include <gen/files.h>
#include <gen/stdfunc.h>
#include <gen/timefunc.h>
#include <widgets/epopup.h>
#include <widgets/etableview.h>
#include <widgets/wd_func.h>

StartupKTFDialog::StartupKTFDialog(Device::CurrentDevice *device, QWidget *parent)
    : AbstractStartupDialog(device, parent)
{
    Bd9Block = new Bd9;
    WBd7Block = new WBd7;
}

StartupKTFDialog::~StartupKTFDialog()
{
    delete Bd9Block;
    delete WBd7Block;
}

void StartupKTFDialog::SetupUI()
{

    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    ETableView *tv = new ETableView;
    tv->setObjectName("cor");
    int row = 0;

    glyout->addWidget(WDFunc::NewLBL2(this, "Текущий расход ресурса изоляции, час:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB2(this, QString::number(907), 0, 1000000, 5), row, 2, 1, 2);
    row++;

    QPushButton *pb = new QPushButton("Прочитать из модуля");
    // NOTE Use button widget instead
    //   connect(pb, &QAbstractButton::clicked, this, &AbstractStartupDialog::GetCorBd);

    glyout->addWidget(pb, row, 1, 1, 2);

    pb = new QPushButton("Записать в модуль");
    connect(pb, &QAbstractButton::clicked, this, &StartupKTFDialog::WriteCor);

    glyout->addWidget(pb, row, 3, 1, 2);

    row++;

    pb = new QPushButton("Прочитать значения из файла");
    connect(pb, &QAbstractButton::clicked, this, &StartupKTFDialog::ReadFromFile);

    glyout->addWidget(pb, row, 1, 1, 2);

    pb = new QPushButton("Сохранить значения в файл");
    connect(pb, &QAbstractButton::clicked, this, &StartupKTFDialog::SaveToFile);

    glyout->addWidget(pb, row, 3, 1, 2);

    lyout->addLayout(glyout, Qt::AlignTop);
    lyout->addWidget(tv, 89);
    setLayout(lyout);
}

// void StartupKTFDialog::FillBackCor()
//{
//    WDFunc::SPBData(this, QString::number(907), WBd7Block->InitAge);
//}

// void StartupKTFDialog::FillCor()
//{
//    WDFunc::SetSPBData(this, QString::number(907), Bd9Block->Age);
//}

// void StartupKTFDialog::GetCorBd()
//{
//    if (index == corDIndex)
//        switch (Board::GetInstance().interfaceType())
//        {
//        case Board::InterfaceType::USB:
//        {
//            if (Commands::GetBd(9, Bd9Block, sizeof(Bd9)) == Error::Msg::NoError)
//            {
//                FillCor();
//                QMessageBox::information(this, "INFO", "Прочитано успешно");
//            }
//            break;
//        }
//        case Board::InterfaceType::Ethernet:
//        {
//            emit CorReadRequest();
//            break;
//        }
//        default:
//            break;
//        }
//}

// void StartupKTFDialog::GetCorBdButton()
//{
//    switch (Board::GetInstance().interfaceType())
//    {
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
//    case Board::InterfaceType::RS485:
//        // else if (MainInterface == I_RS485)
//        {
//            ModBus::Information info;
//            info.size = (sizeof(Bd9) / 4);
//            info.adr = 4000;
//            emit RS485ReadCorBd(info);
//            break;
//        }
//    case Board::InterfaceType::Ethernet:
//        // else if (MainInterface == I_ETHERNET)
//        {
//            emit CorReadRequest();
//            break;
//        }
//    }
//}

// void StartupKTFDialog::WriteCor()
//{
//    int i;
//    quint32 adr = 907;
//    FillBackCor();
//    if (!checkPassword())
//        return;
//    switch (Board::GetInstance().interfaceType())
//    {
//    case Board::InterfaceType::Ethernet:
//    {
//        float corblocki;
//        memcpy(&corblocki, reinterpret_cast<float *>(WBd7Block), sizeof(float));
//        emit SendCom50(adr, corblocki);
//        TimeFunc::Wait(300);
//        break;
//    }
//    case Board::InterfaceType::RS485:
//    {
//        ModBus::Information info;
//        info.size = (sizeof(WBd7) / 4);
//        info.adr = adr;
//        emit RS485WriteCorBd(info, (float *)WBd7Block);
//        break;
//    }
//    case Board::InterfaceType::USB:
//    {
//        if (Commands::WriteBd(7, WBd7Block, sizeof(WBd7)) == Error::Msg::NoError)
//            QMessageBox::information(this, "INFO", "Записано успешно");
//        else
//            QMessageBox::information(this, "INFO", "Ошибка");
//        //......
//        QThread::sleep(1);
//        if (Commands::GetBd(9, Bd9Block, sizeof(Bd9Block)) == Error::Msg::NoError)
//            FillCor();
//        break;
//    }
//    default:
//        break;
//    }
//}

// void StartupKTFDialog::SetupCor()
//{
//}

// void StartupKTFDialog::ResetCor()
//{
//}

void StartupKTFDialog::SaveToFile()
{

    auto filepath = WDFunc::ChooseFileForSave(this, "Tune files (*.cor)", "cor");
    if (filepath.isEmpty())
        return;

    QByteArray ba = QByteArray::fromRawData(reinterpret_cast<char *>(WBd7Block), sizeof(WBd7Block));
    FillBackCor();
    Error::Msg res = Files::SaveToFile(filepath, ba);
    switch (res)
    {
    case Error::Msg::NoError:
        EMessageBox::information(this, "Файл коэффициентов коррекции записан успешно!");
        break;
    case Error::Msg::FileWriteError:
        EMessageBox::error(this, "Ошибка при записи файла!");
        break;
    case Error::Msg::FileOpenError:
        EMessageBox::error(this, "Ошибка открытия файла!");
        break;
    case Error::Msg::FileNameError:
        EMessageBox::warning(this, "Задано пустое имя файла!");
        break;
    default:
        break;
    }
}

void StartupKTFDialog::ReadFromFile()
{
    auto filepath = WDFunc::ChooseFileForOpen(this, "Tune files (*.cor)");
    if (filepath.isEmpty())
        return;

    QByteArray ba;
    ba.resize(sizeof(*Bd9Block));
    Error::Msg res = Files::LoadFromFile(filepath, ba);
    if (res != Error::Msg::NoError)
    {
        EMessageBox::error(this, "Ошибка при загрузке файла");
        ERMSG("Ошибка при загрузке файла");
        return;
    }

    //  FillCor();
    //    memcpy(Bd9Block, &(ba.data()[0]), sizeof(*Bd9Block));
    memcpy(&Bd9Block->Age, &(ba.data()[0]), sizeof(float));
    FillCor();
    QMessageBox::information(this, "Внимание", "Загрузка прошла успешно!");
}

bool StartupKTFDialog::checkStartupValues()
{
    return true;
}

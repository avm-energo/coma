#include "startupkivdialog.h"

#include "../dialogs/keypressdialog.h"
#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/datamanager.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../widgets/wd_func.h"

#include <QDebug>
#include <QGridLayout>
#include <QMessageBox>

StartupKIVDialog::StartupKIVDialog(QWidget *parent) : AbstractStartupDialog(parent)
{
    // Default initialization
    // Do not need set null value
    CorBlock = new CorData;
    addReg(4000, &CorBlock->C_init[0]);
    addReg(4001, &CorBlock->C_init[1]);
    addReg(4002, &CorBlock->C_init[2]);
    addReg(4003, &CorBlock->Tg_init[0]);
    addReg(4004, &CorBlock->Tg_init[1]);
    addReg(4005, &CorBlock->Tg_init[2]);
    addReg(4006, &CorBlock->corTg[0]);
    addReg(4007, &CorBlock->corTg[1]);
    addReg(4008, &CorBlock->corTg[2]);
    addReg(4009, &CorBlock->Iunb_init);
    addReg(4010, &CorBlock->Phy_unb_init);

    SetStartupBlock(7, &CorBlock, sizeof(CorData), KIVSTARTUPINITREG);
}

StartupKIVDialog::~StartupKIVDialog()
{
    delete CorBlock;
}

void StartupKIVDialog::SetupCor()
{
    auto spinBoxes = findChildren<QDoubleSpinBox *>();
    for (const auto spinBox : spinBoxes)
    {
        if (spinBox->value())
        {
            QString message(tr("Сбросьте начальные значения и подождите 30 секунд\n"
                               "После чего повторите операцию задания начальных значений"));
            QMessageBox::warning(this, tr("Начальные значения"), message);
            return;
        }
    }
    AbstractStartupDialog::SetupCor();
}

void StartupKIVDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    int row = 0;

    glyout->addWidget(WDFunc::NewLBL2(this, "Начальные значения емкостей вводов, пФ:"), row, 1, 1, 1);

    for (int i = 0; i < 3; i++)
    {
        glyout->addWidget(WDFunc::NewSPB2(this, QString::number(4000 + i), 0, 10000, 1), row, 2 + i, 1, 1);
    }

    row++;

    glyout->addWidget(WDFunc::NewLBL2(this, "Начальные значения tg δ вводов, %:"), row, 1, 1, 1);

    for (int i = 0; i < 3; i++)
    {
        glyout->addWidget(WDFunc::NewSPB2(this, QString::number(4003 + i), -10, 10, 2), row, 2 + i, 1, 1);
    }

    row++;

    glyout->addWidget(WDFunc::NewLBL2(this, "Коррекция  tg δ вводов,%:"), row, 1, 1, 1);

    for (int i = 0; i < 3; i++)
    {
        glyout->addWidget(WDFunc::NewSPB2(this, QString::number(4006 + i), -10, 10, 2), row, 2 + i, 1, 1);
    }
    row++;

    glyout->addWidget(WDFunc::NewLBL2(this, "Начальное значение небаланса токов, %:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB2(this, QString::number(4009), 0, 10000, 1), row, 2, 1, 3);

    row++;
    glyout->addWidget(WDFunc::NewLBL2(this, "Начальное значение угла тока небаланса, град.:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB2(this, QString::number(4010), 0, 10000, 1), row, 2, 1, 3);

    //    row++;

    lyout->addLayout(glyout, Qt::AlignTop);
    lyout->addWidget(buttonWidget());

    setLayout(lyout);
    setObjectName("corDialog");
    connect(&DataManager::GetInstance(), &DataManager::floatReceived, this, &UWidget::updateFloatData,
        Qt::QueuedConnection);
}

void StartupKIVDialog::SaveToFile()
{
    QByteArray ba = QByteArray::fromRawData(reinterpret_cast<char *>(CorBlock), sizeof(CorData));
    FillBackCor();

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

void StartupKIVDialog::ReadFromFile()
{
    QByteArray ba;
    ba.resize(sizeof(CorData));

    Error::Msg res = Files::LoadFromFile(WDFunc::ChooseFileForOpen(this, "Tune files (*.cor)"), ba);
    if (res != Error::Msg::NoError)
    {
        QMessageBox::critical(this, "Ошибка", "Ошибка при загрузке файла");
        ERMSG("Ошибка при загрузке файла");
        return;
    }
    // CorBlock = reinterpret_cast<CorData *>(ba.data());
    memcpy(CorBlock, &(ba.data()[0]), sizeof(CorData));

    FillCor();
    QMessageBox::information(this, "Внимание", "Загрузка прошла успешно!");
}

void StartupKIVDialog::ErrorRead()
{
    QMessageBox::information(this, "Ошибка", "Ошибка чтения");
}

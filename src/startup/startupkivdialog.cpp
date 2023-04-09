#include "startupkivdialog.h"

#include "../widgets/wd_func.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <gen/error.h>
#include <gen/files.h>
#include <tuple>

StartupKIVDialog::StartupKIVDialog(QWidget *parent) : AbstractStartupDialog(parent)
{
    // Default initialization
    // Do not need set null value
    CorBlock = new CorData;
    addReg(KIVSTARTUPINITREGW, KIVSTARTUPINITREGR, &CorBlock->C_init[0]);
    addReg(KIVSTARTUPINITREGW + 1, KIVSTARTUPINITREGR + 1, &CorBlock->C_init[1]);
    addReg(KIVSTARTUPINITREGW + 2, KIVSTARTUPINITREGR + 2, &CorBlock->C_init[2]);
    addReg(KIVSTARTUPINITREGW + 3, KIVSTARTUPINITREGR + 3, &CorBlock->Tg_init[0]);
    addReg(KIVSTARTUPINITREGW + 4, KIVSTARTUPINITREGR + 4, &CorBlock->Tg_init[1]);
    addReg(KIVSTARTUPINITREGW + 5, KIVSTARTUPINITREGR + 5, &CorBlock->Tg_init[2]);
    addReg(KIVSTARTUPINITREGW + 6, KIVSTARTUPINITREGR + 6, &CorBlock->corTg[0]);
    addReg(KIVSTARTUPINITREGW + 7, KIVSTARTUPINITREGR + 7, &CorBlock->corTg[1]);
    addReg(KIVSTARTUPINITREGW + 8, KIVSTARTUPINITREGR + 8, &CorBlock->corTg[2]);
    addReg(KIVSTARTUPINITREGW + 9, KIVSTARTUPINITREGR + 9, &CorBlock->Iunb_init);
    addReg(KIVSTARTUPINITREGW + 10, KIVSTARTUPINITREGR + 10, &CorBlock->Phy_unb_init);
    SetStartupBlock(7, &CorBlock, sizeof(CorData), KIVSTARTUPINITREGR);
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

QWidget *StartupKIVDialog::uiValuesTab(QWidget *parent)
{
    auto widget = new QWidget(parent);
    auto layout = new QVBoxLayout;
    auto glyout = new QGridLayout;
    int row = 0;

    glyout->addWidget(WDFunc::NewLBL2(this, "Начальные значения емкостей вводов, пФ:"), row, 1, 1, 1);
    for (int i = 0; i < 3; i++)
    {
        glyout->addWidget(
            WDFunc::NewSPB2(this, QString::number(KIVSTARTUPINITREGR + i), 0, 10000, 1), row, 2 + i, 1, 1);
    }
    row++;

    glyout->addWidget(WDFunc::NewLBL2(this, "Начальные значения tg δ вводов, %:"), row, 1, 1, 1);
    for (int i = 0; i < 3; i++)
    {
        glyout->addWidget(
            WDFunc::NewSPB2(this, QString::number(KIVSTARTUPINITREGR + 3 + i), -10, 10, 2), row, 2 + i, 1, 1);
    }
    row++;

    glyout->addWidget(WDFunc::NewLBL2(this, "Коррекция  tg δ вводов,%:"), row, 1, 1, 1);
    for (int i = 0; i < 3; i++)
    {
        glyout->addWidget(
            WDFunc::NewSPB2(this, QString::number(KIVSTARTUPINITREGR + 6 + i), -10, 10, 2), row, 2 + i, 1, 1);
    }
    row++;

    glyout->addWidget(WDFunc::NewLBL2(this, "Начальное значение небаланса токов, %:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB2(this, QString::number(KIVSTARTUPINITREGR + 9), 0, 10000, 1), row, 2, 1, 3);
    row++;
    glyout->addWidget(WDFunc::NewLBL2(this, "Начальное значение угла тока небаланса, град.:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB2(this, QString::number(KIVSTARTUPINITREGR + 10), 0, 10000, 1), row, 2, 1, 3);

    layout->addLayout(glyout, Qt::AlignTop);
    layout->addWidget(buttonWidget());

    widget->setLayout(layout);
    widget->setObjectName("corDialog");
    return widget;
}

QWidget *StartupKIVDialog::uiCommandsTab(QWidget *parent)
{
    auto widget = new QWidget(parent);
    auto layout = new QVBoxLayout(widget);

    // Create UI commands for phases
    const QList<std::tuple<Queries::Commands, QString>> phaseSettings {
        { Queries::QC_SetStartupPhaseA, "A" }, //
        { Queries::QC_SetStartupPhaseB, "B" }, //
        { Queries::QC_SetStartupPhaseC, "C" }  //
    };
    for (auto &step : phaseSettings)
    {
        auto setupCmd = std::get<0>(step);
        auto &phase = std::get<1>(step);
        auto setupValues = new QPushButton(QString("Задать начальные значения по фазе %1").arg(phase), widget);
        connect(setupValues, &QPushButton::clicked, this, [this, setupCmd]() { sendCommand(setupCmd); });
        layout->addWidget(setupValues);
    }

    // Create UI commands for unbalance current
    {
        auto setupValues = new QPushButton("Задать начальные значения небаланса токов", widget);
        connect(setupValues, &QPushButton::clicked, this, //
            [this]() {
                auto assocFields = findChildren<QDoubleSpinBox *>(QString::number(KIVSTARTUPINITREGR + 9));
                assocFields.append(findChildren<QDoubleSpinBox *>(QString::number(KIVSTARTUPINITREGR + 10)));
                for (auto field : assocFields)
                {
                    if (field->value())
                    {
                        QString message(tr("Сбросьте начальные значения и подождите 30 секунд\n"
                                           "После чего повторите операцию задания начальных значений"));
                        QMessageBox::warning(this, tr("Начальные значения"), message);
                        return;
                    }
                }
                sendCommand(Queries::QC_SetStartupUnbounced);
            });
        layout->addWidget(setupValues);

        auto clearValues = new QPushButton("Сбросить начальные значения небаланса токов", widget);
        connect(clearValues, &QPushButton::clicked, this, [this]() { sendCommand(Queries::QC_ClearStartupUnbounced); });
        layout->addWidget(clearValues);
    }

    // Create UI commands trans off and reset starup init error
    {
        auto resetStartupErr = new QPushButton("Сброс ошибки задания начальных значений", widget);
        connect(resetStartupErr, &QPushButton::clicked, this, [this]() { sendCommand(Queries::QC_ClearStartupError); });
        layout->addWidget(resetStartupErr);

        auto setTransOff = new QPushButton("Послать команду \"Трансформатор включён\"", widget);
        connect(setTransOff, &QPushButton::clicked, this, [this]() { sendCommand(Queries::QC_SetTransOff, false); });
        layout->addWidget(setTransOff);

        auto setTransOn = new QPushButton("Послать команду \"Трансформатор отключён\"", widget);
        connect(setTransOn, &QPushButton::clicked, this, [this]() { sendCommand(Queries::QC_SetTransOff); });
        layout->addWidget(setTransOn);
    }

    widget->setLayout(layout);
    widget->setObjectName("commandsTab");
    return widget;
}

void StartupKIVDialog::sendCommand(Queries::Commands cmd, bool value)
{
    if (checkPassword())
    {
        BaseInterface::iface()->writeCommand(cmd, value);
        GetCorBd();
    }
}

void StartupKIVDialog::SetupUI()
{
    auto layout = new QVBoxLayout;
    auto startupTabWidget = new QTabWidget(this);
    startupTabWidget->addTab(uiCommandsTab(startupTabWidget), "Команды");
    startupTabWidget->addTab(uiValuesTab(startupTabWidget), "Значения");
    layout->addWidget(startupTabWidget);
    setLayout(layout);
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

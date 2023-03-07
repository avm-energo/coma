#include "startupkivdialog.h"

#include "../widgets/wd_func.h"

#include <QGridLayout>
#include <QMessageBox>
#include <QPushButton>
#include <gen/error.h>
#include <gen/files.h>

//#include "../module/board.h"
//#include "../dialogs/keypressdialog.h"
//#include <QDebug>
//#include <gen/colors.h>
//#include <gen/datamanager/typesproxy.h>
//#include <gen/stdfunc.h>
//#include <gen/timefunc.h>

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

QWidget *StartupKIVDialog::uiValuesTab(QWidget *parent)
{
    auto widget = new QWidget(parent);
    auto layout = new QVBoxLayout;
    auto glyout = new QGridLayout;
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
    auto grid = new QGridLayout(widget);

    // Prepare UI step
    constexpr auto columns = 4;
    constexpr auto rows = 10;
    constexpr auto phasesCount = 3;
    const char *phases[] = { "A", "B", "C" };
    // Labels for columns
    QList<QWidget *> labelList = {
        WDFunc::NewLBL2(widget, "Начальные значения емкостей вводов, пФ:"), //
        WDFunc::NewLBL2(widget, "Начальные значения tg δ вводов, %:"),      //
        WDFunc::NewLBL2(widget, "Коррекция  tg δ вводов,%:")                //
    };
    // Adding column labels to grid layout
    for (auto col = 1; col < columns; col++)
        grid->addWidget(labelList[col - 1], 0, col, 1, 1, Qt::AlignTop);

    const QList<std::tuple<double, double, int>> spinBoxSettings = { { 0, 10000, 1 }, { -10, 10, 2 }, { -10, 10, 2 } };

    // Create Matrix-styled UI
    for (auto phase = 0; phase < phasesCount; phase++)
    {
        auto phaseLable = WDFunc::NewLBL2(widget, QString("Фаза %1").arg(phases[phase]));
        const auto row = phase * phasesCount + 1;
        grid->addWidget(phaseLable, row, 0, 1, 1, Qt::AlignTop);
        for (auto col = 1; col < columns; col++)
        {
            const auto index = col - 1;
            const auto offset = (phasesCount * index) + phase;
            auto &min = std::get<0>(spinBoxSettings[index]);
            auto &max = std::get<1>(spinBoxSettings[index]);
            auto &decimals = std::get<2>(spinBoxSettings[index]);
            auto spinBox = WDFunc::NewSPB2(widget, QString::number(KIVSTARTUPINITREG + offset), min, max, decimals);
            grid->addWidget(spinBox, row, col, 1, 1, Qt::AlignTop);
        }
        auto setupValues = new QPushButton(QString("Задать начальные значения по фазе %1").arg(phases[phase]), widget);
        // TODO: connect
        grid->addWidget(setupValues, row + 1, 0, 1, columns, Qt::AlignTop);

        auto resetValues
            = new QPushButton(QString("Сбросить начальные значения по фазе %1").arg(phases[phase]), widget);
        // TODO: connect
        grid->addWidget(resetValues, row + 2, 0, 1, columns, Qt::AlignTop);
    }

    /// TODO: Здесь будет готовый код
    // auto resetStartupErrorBtn = new QPushButton("Сброс ошибки задания начальных значений", widget);
    // grid->addWidget(resetStartupErrorBtn, 0, 0, 1, 1);

    // auto transOffBtn = new QPushButton("Команда \"Трансформатор отключён\"", widget);
    // grid->addWidget(transOffBtn, 1, 0, 1, 1);

    layout->addLayout(grid, Qt::AlignTop);
    widget->setLayout(layout);
    widget->setObjectName("commandsTab");
    return widget;
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

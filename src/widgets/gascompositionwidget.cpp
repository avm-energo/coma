#include "gascompositionwidget.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <gen/std_ext.h>

GasCompositionWidget::GasCompositionWidget(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

QLineEdit *GasCompositionWidget::createLineEdit(float min, float max, int decimals, const QString &text)
{
    auto lineEdit = new QLineEdit(text, this);
    auto validator = new QDoubleValidator(this);
    validator->setNotation(QDoubleValidator::Notation::StandardNotation);
    validator->setRange(min, max, decimals);
    lineEdit->setValidator(validator);
    return lineEdit;
}

QComboBox *GasCompositionWidget::createGasTypeWidget(std::size_t index)
{
    static const std::vector<std::pair<GasType, QString>> typeItems {
        { GasType::NotChosen, "Не выбран" },                      //
        { GasType::SulfurHexafluoride, "Элегаз (SF6)" },          //
        { GasType::CarbonTetrafluoride, "Тетрафторметан (CF4)" }, //
        { GasType::Hexafluoroethane, "Гексафторэтан (C2F6)" },    //
        { GasType::Nitrogen, "Азот (N2)" },                       //
        { GasType::Oxygen, "Кислород (О2)" },                     //
        { GasType::Argon, "Аргон (Ar)" },                         //
        { GasType::HydrogenFluoride, "Фтороводород (HF)" },       //
        { GasType::Other, "Другой газ" }                          //
    };

    auto gasTypeWidget = new QComboBox(this);
    for (const auto &[type, name] : typeItems)
        gasTypeWidget->insertItem(std_ext::to_underlying(type), name);
    gasTypeWidget->setCurrentIndex(std_ext::to_underlying(GasType::NotChosen));
    connect(gasTypeWidget, qOverload<int>(&QComboBox::currentIndexChanged), this, //
        [this, index](const int newIndex) {
            /// TODO: Костыль, убрать ;)
            constexpr int otherIndex = std_ext::to_underlying(GasType::HydrogenFluoride) + 1;
            if (newIndex == otherIndex)
                gasTypeChanged(index, GasType::Other);
            else
                gasTypeChanged(index, GasType(newIndex));
        });
    return gasTypeWidget;
}

QLineEdit *GasCompositionWidget::createMolarMassWidget(std::size_t index)
{
    auto molarMassWidget = new QLineEdit("0,0", this);
    auto validator = new QDoubleValidator(this);
    validator->setNotation(QDoubleValidator::Notation::StandardNotation);
    validator->setRange(0.0, 500.0, 10);
    molarMassWidget->setValidator(validator);
    connect(molarMassWidget, &QLineEdit::textEdited, this, //
        [this, index](const QString &newValue) {
            bool convertStatus = false;
            auto newMolarMass = newValue.toFloat(&convertStatus);
            if (convertStatus)
                molarMassChanged(index, newMolarMass);
        });
    return molarMassWidget;
}

QLineEdit *GasCompositionWidget::createMassWidget(std::size_t index)
{
    auto massWidget = new QLineEdit("0,0", this);
    auto validator = new QDoubleValidator(this);
    validator->setNotation(QDoubleValidator::Notation::StandardNotation);
    validator->setRange(0.0, 500.0, 10);
    massWidget->setValidator(validator);
    connect(massWidget, &QLineEdit::textEdited, this, //
        [this, index](const QString &newValue) {
            bool convertStatus = false;
            auto newMass = newValue.toFloat(&convertStatus);
            if (convertStatus)
                massChanged(index, newMass);
        });
    return massWidget;
}

QLineEdit *GasCompositionWidget::createMoleFracWidget(std::size_t index)
{
    auto moleFracWidget = new QLineEdit("0,0", this);
    auto validator = new QDoubleValidator(this);
    validator->setNotation(QDoubleValidator::Notation::StandardNotation);
    validator->setRange(0.0, 100.0, 4);
    moleFracWidget->setValidator(validator);
    connect(moleFracWidget, &QLineEdit::textEdited, this, //
        [this, index](const QString &newValue) {
            bool convertStatus = false;
            auto newMoleFrac = newValue.toFloat(&convertStatus);
            if (convertStatus)
                moleFracChanged(index, newMoleFrac);
        });
    return moleFracWidget;
}

void GasCompositionWidget::setupUI()
{
    // Начальное создание
    auto layout = new QGridLayout;
    std::size_t row = 0, column = 1;
    auto gasTypeLabel = new QLabel("Тип газа", this);
    layout->addWidget(gasTypeLabel, row, column++, Qt::AlignCenter);
    auto molarMassLabel = new QLabel("Молярная масса, г/моль", this);
    layout->addWidget(molarMassLabel, row, column++, Qt::AlignCenter);
    auto massLabel = new QLabel("Масса, кг", this);
    layout->addWidget(massLabel, row, column++, Qt::AlignCenter);
    auto moleFracLabel = new QLabel("Мольная доля, %", this);
    layout->addWidget(moleFracLabel, row, column++, Qt::AlignCenter);

    // Создание активных виджетов для ввода данных
    std::size_t index = 0;
    for (auto &widgetRow : widgetRows)
    {
        column = 1;
        row = index + 1;
        widgetRow.gasTypeInput = createGasTypeWidget(index);
        layout->addWidget(widgetRow.gasTypeInput, row, column++, Qt::AlignCenter);
        widgetRow.molarMassInput = createMolarMassWidget(index);
        layout->addWidget(widgetRow.molarMassInput, row, column++, Qt::AlignCenter);
        widgetRow.massInput = createMassWidget(index);
        layout->addWidget(widgetRow.massInput, row, column++, Qt::AlignCenter);
        widgetRow.moleFracInput = createMoleFracWidget(index);
        layout->addWidget(widgetRow.moleFracInput, row, column++, Qt::AlignCenter);
        ++index;
    }
    setLayout(layout);
}

void GasCompositionWidget::gasTypeChanged(std::size_t index, GasType newGasType)
{
    Q_UNUSED(index);
    Q_UNUSED(newGasType);
}

void GasCompositionWidget::molarMassChanged(std::size_t index, float newMolarMass)
{
    Q_UNUSED(index);
    Q_UNUSED(newMolarMass);
}

void GasCompositionWidget::massChanged(std::size_t index, float newMass)
{
    Q_UNUSED(index);
    Q_UNUSED(newMass);
}

void GasCompositionWidget::moleFracChanged(std::size_t index, float newMoleFrac)
{
    Q_UNUSED(index);
    Q_UNUSED(newMoleFrac);
}

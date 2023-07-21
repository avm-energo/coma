#include "gascompositionwidget.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <gen/std_ext.h>

bool GasWidgetRow::isActive() const noexcept
{
    return (massInput->isEnabled() || moleFracInput->isEnabled());
}

float GasWidgetRow::getLineEditData(const QLineEdit *lineEdit) const noexcept
{
    bool convertStatus = false;
    const auto dataStr = lineEdit->text();
    auto data = QLocale::system().toFloat(dataStr, &convertStatus);
    if (convertStatus)
        return data;
    else
        return 0;
}

void GasWidgetRow::setLineEditData(const float data, QLineEdit *lineEdit) noexcept
{
    const auto dataString = QLocale::system().toString(data);
    lineEdit->setText(dataString);
}

void GasWidgetRow::layoutAction(const GasType gasType, const InputMode inputMode) noexcept
{
    if (gasType == GasType::NotChosen)
    {
        molarMassInput->setEnabled(false);
        massInput->setEnabled(false);
        moleFracInput->setEnabled(false);
    }
    else
    {
        if (gasType == GasType::Other)
            molarMassInput->setEnabled(true);
        else
            molarMassInput->setEnabled(false);
        if (inputMode == InputMode::InputMass)
            massInput->setEnabled(true);
        else
            massInput->setEnabled(false);
        if (inputMode == InputMode::InputMoleFrac)
            moleFracInput->setEnabled(true);
        else
            moleFracInput->setEnabled(false);
    }
}

float GasWidgetRow::getMolarMass() const noexcept
{
    return getLineEditData(molarMassInput);
}

float GasWidgetRow::getMass() const noexcept
{
    return getLineEditData(massInput);
}

float GasWidgetRow::getMoleFrac() const noexcept
{
    return getLineEditData(moleFracInput);
}

float GasWidgetRow::getMoles() const noexcept
{
    const auto molarMass = getMolarMass();
    const auto mass = getMass();
    constexpr float kilo = 1000;
    if (molarMass != 0)
        return (mass * 1000) / molarMass;
    else
        return 0;
}

void GasWidgetRow::setMolarMass(const float molarMass) noexcept
{
    setLineEditData(molarMass, molarMassInput);
}

void GasWidgetRow::setMass(const float mass) noexcept
{
    setLineEditData(mass, massInput);
}

void GasWidgetRow::setMoleFrac(const float moleFrac) noexcept
{
    setLineEditData(moleFrac, moleFracInput);
}

void GasWidgetRow::setTotalMoles(const float totalMoles) noexcept
{
    constexpr float one_hundred = 100;
    const auto moles = getMoles();
    const auto moleFrac = (moles / totalMoles) * one_hundred;
    setMoleFrac(moleFrac);
}

GasCompositionWidget::GasCompositionWidget(QWidget *parent) : QWidget(parent), workMode(InputMode::InputMass)
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

QLineEdit *GasCompositionWidget::createMolarMassWidget()
{
    auto molarMassWidget = createLineEdit(0, 500);
    connect(molarMassWidget, &QLineEdit::textEdited, this, //
        [this]([[maybe_unused]] const QString &newValue) { recalc(); });
    return molarMassWidget;
}

QLineEdit *GasCompositionWidget::createMassWidget()
{
    auto massWidget = createLineEdit(0, 100);
    connect(massWidget, &QLineEdit::textEdited, this, //
        [this]([[maybe_unused]] const QString &newValue) { recalc(); });
    return massWidget;
}

QLineEdit *GasCompositionWidget::createMoleFracWidget(std::size_t index)
{
    auto moleFracWidget = createLineEdit(0, 100, 3);
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

    auto gasMassModeBtn = new QRadioButton("Задание масс газов", this);
    gasMassModeBtn->setChecked(true);
    connect(gasMassModeBtn, &QRadioButton::clicked, this, [this] { inputModeChanged(InputMode::InputMass); });
    layout->addWidget(gasMassModeBtn, row, 0, 1, 3, Qt::AlignCenter);
    auto moleFracModeBtn = new QRadioButton("Задание молярных долей газов", this);
    connect(moleFracModeBtn, &QRadioButton::clicked, this, [this] { inputModeChanged(InputMode::InputMoleFrac); });
    layout->addWidget(moleFracModeBtn, row++, 3, 1, 2, Qt::AlignCenter);
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
        column = 0;
        row = index + 2;
        auto gasLabel = new QLabel("Газ " + QString::number(row - 1), this);
        layout->addWidget(gasLabel, row, column++, Qt::AlignCenter);
        widgetRow.gasTypeInput = createGasTypeWidget(index);
        layout->addWidget(widgetRow.gasTypeInput, row, column++, Qt::AlignCenter);
        widgetRow.molarMassInput = createMolarMassWidget();
        widgetRow.molarMassInput->setEnabled(false);
        layout->addWidget(widgetRow.molarMassInput, row, column++, Qt::AlignCenter);
        widgetRow.massInput = createMassWidget();
        widgetRow.massInput->setEnabled(false);
        layout->addWidget(widgetRow.massInput, row, column++, Qt::AlignCenter);
        widgetRow.moleFracInput = createMoleFracWidget(index);
        widgetRow.moleFracInput->setEnabled(false);
        layout->addWidget(widgetRow.moleFracInput, row, column++, Qt::AlignCenter);
        ++index;
    }
    setLayout(layout);
}

void GasCompositionWidget::inputModeChanged(const InputMode newInputMode)
{
    // Если вводим массы газов
    if (newInputMode == InputMode::InputMass)
    {
        for (auto &widgetRow : widgetRows)
        {
            auto typeIndex = widgetRow.gasTypeInput->currentIndex();
            // Отключаем ввод мольной доли, только отображаем
            widgetRow.moleFracInput->setEnabled(false);
            // Включаем поля ввода массы газа, если выбран тип газа
            if (typeIndex != std_ext::to_underlying(GasType::NotChosen))
                widgetRow.massInput->setEnabled(true);
            widgetRow.setMass(1);
        }
        recalc();
    }
    // Если вводим мольные доли газов
    if (newInputMode == InputMode::InputMoleFrac)
    {
        for (auto &widgetRow : widgetRows)
        {
            auto typeIndex = widgetRow.gasTypeInput->currentIndex();
            // Отключаем ввод массы газа, отображаем всегда "Нет"
            widgetRow.massInput->setEnabled(false);
            widgetRow.massInput->setText("Нет");
            // Включаем поля ввода мольной доли газа, если выбран тип газа
            if (typeIndex != std_ext::to_underlying(GasType::NotChosen))
                widgetRow.moleFracInput->setEnabled(true);
        }
    }
    workMode = newInputMode;
}

void GasCompositionWidget::gasTypeChanged(const std::size_t index, const GasType newGasType)
{
    static const std::map<GasType, float> molarMassMap {
        { GasType::SulfurHexafluoride, 146.06 },  //
        { GasType::CarbonTetrafluoride, 88.004 }, //
        { GasType::Hexafluoroethane, 138.0118 },  //
        { GasType::Nitrogen, 28.0134 },           //
        { GasType::Oxygen, 31.9988 },             //
        { GasType::Argon, 39.948 },               //
        { GasType::HydrogenFluoride, 20.00634 }   //
    };

    auto &widgetRow = widgetRows[index];
    widgetRow.layoutAction(newGasType, workMode);
    // Если выбранный тип соответствует одному из molarMassMap
    if (newGasType != GasType::NotChosen && newGasType != GasType::Other)
    {
        // molarMassInput изменяет содержимое в соответствии с
        // molarMassMap без ручного ввода от пользователя
        auto search = molarMassMap.find(newGasType);
        if (search != molarMassMap.cend())
            widgetRow.setMolarMass(search->second);
        if (workMode == InputMode::InputMass)
            widgetRow.setMass(1);
    }
    if (newGasType == GasType::NotChosen)
    {
        widgetRow.moleFracInput->setText("Нет");
        widgetRow.massInput->setText("Нет");
    }
    if (workMode == InputMode::InputMass)
        recalc();
}

void GasCompositionWidget::recalc()
{
    float totalMoles = 0;
    for (auto &widgetRow : widgetRows)
        if (widgetRow.isActive())
            totalMoles += widgetRow.getMoles();

    for (auto &widgetRow : widgetRows)
        if (widgetRow.isActive())
            widgetRow.setTotalMoles(totalMoles);
}

void GasCompositionWidget::moleFracChanged(const std::size_t index, const float newMoleFrac)
{
    [[maybe_unused]] auto &currentWidgetRow = widgetRows[index];
    Q_UNUSED(newMoleFrac);
    ///
}

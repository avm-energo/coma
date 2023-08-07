#include "gasdensitywidget.h"

#include <QComboBox>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <cmath>
#include <gen/std_ext.h>

bool GasWidgetRow::isActive() const noexcept
{
    return (m_weightInput->isEnabled() || m_moleFracInput->isEnabled());
}

void GasWidgetRow::layoutAction(const GasType gasType, const InputMode inputMode) noexcept
{
    if (gasType == GasType::NotChosen)
    {
        m_molarMassInput->setEnabled(false);
        m_weightInput->setEnabled(false);
        m_moleFracInput->setEnabled(false);
    }
    else
    {
        if (gasType == GasType::Other)
            m_molarMassInput->setEnabled(true);
        else
            m_molarMassInput->setEnabled(false);
        if (inputMode == InputMode::InputMass)
            m_weightInput->setEnabled(true);
        else
            m_weightInput->setEnabled(false);
        if (inputMode == InputMode::InputMoleFrac)
            m_moleFracInput->setEnabled(true);
        else
            m_moleFracInput->setEnabled(false);
    }
}

GasType GasWidgetRow::getGasType() const noexcept
{
    // Костыль, "Другой газ" - всегда последний в combo box
    auto index = m_gasTypeInput->currentIndex();
    if (index == (m_gasTypeInput->count() - 1))
        return GasType::Other;
    else
        return GasType(index);
}

float GasWidgetRow::getMolarMass() const noexcept
{
    return static_cast<float>(m_molarMassInput->value());
}

float GasWidgetRow::getWeight() const noexcept
{
    return static_cast<float>(m_weightInput->value());
}

float GasWidgetRow::getMoleFrac() const noexcept
{
    return static_cast<float>(m_moleFracInput->value());
}

float GasWidgetRow::getMoles() const noexcept
{
    const auto molarMass = getMolarMass();
    const auto weight = getWeight();
    constexpr float kilo = 1000.0f;
    if (molarMass != 0)
        return (weight * kilo) / molarMass;
    else
        return 0;
}

void GasWidgetRow::setGasType(const GasType type) noexcept
{
    // Костыль, "Другой газ" - всегда последний в combo box
    quint32 index = 0;
    if (type == GasType::Other)
        index = (m_gasTypeInput->count() - 1);
    else
        index = std_ext::to_underlying(type);
    m_gasTypeInput->setCurrentIndex(index);
}

void GasWidgetRow::setMolarMass(const float molarMass) noexcept
{
    m_molarMassInput->setValue(molarMass);
}

void GasWidgetRow::setWeight(const float weight) noexcept
{
    m_weightInput->setValue(weight);
}

void GasWidgetRow::setMoleFrac(const float moleFrac) noexcept
{
    m_moleFracInput->setValue(moleFrac);
}

void GasWidgetRow::setTotalMoles(const float totalMoles) noexcept
{
    const auto moles = getMoles();
    float moleFrac = 0.0f;
    if (totalMoles != 0)
        moleFrac = (moles / totalMoles) * 100.0f;
    setMoleFrac(moleFrac);
}

GasDensityWidget::GasDensityWidget(QWidget *parent)
    : QWidget(parent), m_workMode(InputMode::InputMass), m_status(Status::Correct), m_statusWidget(new QLabel("", this))
{
    setupUI();
}

QDoubleSpinBox *GasDensityWidget::createSpinBox(float min, float max, int decimals, float defaultValue)
{
    auto spinBox = new QDoubleSpinBox(this);
    spinBox->setMinimum(min);
    spinBox->setMaximum(max);
    spinBox->setDecimals(decimals);
    spinBox->setValue(defaultValue);
    return spinBox;
}

QComboBox *GasDensityWidget::createGasTypeWidget(std::size_t index)
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
        [this, index]([[maybe_unused]] const auto _) {
            auto newGasType = m_widgetRows[index].getGasType();
            gasTypeChanged(index, newGasType);
        });
    return gasTypeWidget;
}

QDoubleSpinBox *GasDensityWidget::createMolarMassWidget()
{
    auto molarMassWidget = createSpinBox(0, 500);
    connect(molarMassWidget, &QDoubleSpinBox::textChanged, this, //
        [this](const auto &) {
            if (m_workMode == InputMode::InputMass)
                recalc();
        });
    return molarMassWidget;
}

QDoubleSpinBox *GasDensityWidget::createMassWidget()
{
    auto massWidget = createSpinBox(0, 100);
    connect(massWidget, &QDoubleSpinBox::textChanged, this, //
        [this](const auto &) { recalc(); });
    return massWidget;
}

QDoubleSpinBox *GasDensityWidget::createMoleFracWidget(const std::size_t index)
{
    auto moleFracWidget = createSpinBox(0, 100);
    connect(moleFracWidget, &QDoubleSpinBox::textChanged, this, //
        [this, index](const auto &) { recalc(index); });
    return moleFracWidget;
}

void GasDensityWidget::setupUI()
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

    // Создание рядов виджетов для ввода данных
    std::size_t index = 0;
    for (auto &widgetRow : m_widgetRows)
    {
        column = 0;
        row = index + 2;
        auto gasLabel = new QLabel("Газ " + QString::number(row - 1), this);
        layout->addWidget(gasLabel, row, column++, Qt::AlignCenter);
        widgetRow.m_gasTypeInput = createGasTypeWidget(index);
        layout->addWidget(widgetRow.m_gasTypeInput, row, column++, Qt::AlignCenter);
        widgetRow.m_molarMassInput = createMolarMassWidget();
        widgetRow.m_molarMassInput->setEnabled(false);
        layout->addWidget(widgetRow.m_molarMassInput, row, column++, Qt::AlignCenter);
        widgetRow.m_weightInput = createMassWidget();
        widgetRow.m_weightInput->setEnabled(false);
        layout->addWidget(widgetRow.m_weightInput, row, column++, Qt::AlignCenter);
        widgetRow.m_moleFracInput = createMoleFracWidget(index);
        widgetRow.m_moleFracInput->setEnabled(false);
        layout->addWidget(widgetRow.m_moleFracInput, row, column++, Qt::AlignCenter);
        ++index;
    }
    setStatus(Status::Incorrect);
    layout->addWidget(m_statusWidget, ++row, 0, 1, 5, Qt::AlignCenter);
    setLayout(layout);
}

void GasDensityWidget::setStatus(const Status newStatus)
{
    if (m_status != newStatus)
    {
        m_status = newStatus;
        if (m_status == Status::Incorrect)
        {
            m_statusWidget->setText("Incorrect data");
            m_statusWidget->setStyleSheet("QLabel { color : red; }");
        }
        else
        {
            m_statusWidget->setText("Correct data");
            m_statusWidget->setStyleSheet("QLabel { color : green; }");
        }
    }
}

Status GasDensityWidget::checkValues()
{
    Status newStatus = Status::Correct;
    float totalMoleFrac = 0.0f;
    for (auto &widgetRow : m_widgetRows)
    {
        if (widgetRow.isActive())
        {
            const auto moleFrac = widgetRow.getMoleFrac();
            // Суммируем в totalMoleFrac
            totalMoleFrac += moleFrac;
            // Проверка, что каждое поле лежит в диапазоне от 0% до 100% и не является NaN
            if (moleFrac < 0.0f || moleFrac > 100.0f || std::isnan(moleFrac))
            {
                newStatus = Status::Incorrect;
                break;
            }
        }
    }

    // Если прошлая проверка прошла успешно
    if (newStatus == Status::Correct)
    {
        // totalMoleFrac должен быть равен 100%
        constexpr float ten = 10.0f;
        constexpr float epsilon = std::numeric_limits<float>::epsilon() * 100.0f * 10;
        const float diff = std::fabs(totalMoleFrac - 100.0f);
        // Если большая погрешность
        if (diff > epsilon)
            newStatus = Status::Incorrect;
    }
    return newStatus;
}

void GasDensityWidget::inputModeChanged(const InputMode newInputMode)
{
    // Если вводим массы газов
    if (newInputMode == InputMode::InputMass)
    {
        for (auto &widgetRow : m_widgetRows)
        {
            auto typeIndex = widgetRow.m_gasTypeInput->currentIndex();
            // Отключаем ввод мольной доли, только отображаем
            widgetRow.m_moleFracInput->setEnabled(false);
            // Включаем поля ввода массы газа, если выбран тип газа
            if (typeIndex != std_ext::to_underlying(GasType::NotChosen))
                widgetRow.m_weightInput->setEnabled(true);
            widgetRow.setWeight(1);
        }
        recalc();
    }
    // Если вводим мольные доли газов
    if (newInputMode == InputMode::InputMoleFrac)
    {
        for (auto &widgetRow : m_widgetRows)
        {
            auto typeIndex = widgetRow.m_gasTypeInput->currentIndex();
            // Отключаем ввод массы газа, отображаем всегда "Нет"
            widgetRow.m_weightInput->setEnabled(false);
            widgetRow.m_weightInput->setValue(0);
            // Включаем поля ввода мольной доли газа, если выбран тип газа
            if (typeIndex != std_ext::to_underlying(GasType::NotChosen))
            {
                widgetRow.m_moleFracInput->setEnabled(true);
                widgetRow.m_moleFracInput->setValue(0);
            }
        }
    }
    m_workMode = newInputMode;
}

void GasDensityWidget::gasTypeChanged(const std::size_t index, const GasType newGasType)
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

    auto &widgetRow = m_widgetRows[index];
    widgetRow.layoutAction(newGasType, m_workMode);
    // Если выбранный тип соответствует одному из molarMassMap
    if (newGasType != GasType::NotChosen && newGasType != GasType::Other)
    {
        // molarMassInput изменяет содержимое в соответствии с
        // molarMassMap без ручного ввода от пользователя
        auto search = molarMassMap.find(newGasType);
        if (search != molarMassMap.cend())
            widgetRow.setMolarMass(search->second);
        if (m_workMode == InputMode::InputMass)
            widgetRow.setWeight(1);
    }
    if (newGasType == GasType::NotChosen)
    {
        widgetRow.m_moleFracInput->setValue(0);
        widgetRow.m_weightInput->setValue(0);
    }
    if (m_workMode == InputMode::InputMass)
        recalc();
    else
    {
        if (newGasType != GasType::NotChosen)
            widgetRow.setMoleFrac(0);
        recalc(index);
    }
}

void GasDensityWidget::recalc()
{
    float totalMoles = 0;
    for (auto &widgetRow : m_widgetRows)
        if (widgetRow.isActive())
            totalMoles += widgetRow.getMoles();

    for (auto &widgetRow : m_widgetRows)
        if (widgetRow.isActive())
            widgetRow.setTotalMoles(totalMoles);

    setStatus(checkValues());
}

void GasDensityWidget::recalc(const std::size_t indexChanged)
{
    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *                       ОЧЕНЬ ИНТЕРЕСНАЯ И ВАЖНАЯ ИНФОРМАЦИЯ                        *
     * Ряд виджетов (widget row) считается активным, если выбран тип газа или определён  *
     * другой газ с заданной пользователем молярной массой. Если газ не выбран, то ряд   *
     * виджетов считается неактивным. Описанное поведение реализуется методом isActive.  *
     * Поведение всего виджета (GasCompositionWidget) при изменении мольной доли газа    *
     * зависит от количества активных рядов виджетов:                                    *
     * - имеется 1 активный ряд виджетов: его мольная доля всегда равна 100%, и          *
     * пользователь пытается редактировать это значение. В таком случае автоматически    *
     * подставляются 100%;                                                               *
     * - имеется 2 активных ряда виджетов: если меняются данные 1ого ряда, то данные     *
     * во 2ом ряде должна подставиться автоматически и равняться:                        *
     * 100% - данные мольной доли газа из 1ого ряда;                                     *
     * - имеется 3 активных ряда виджетов: данные 1ого ряда являются статическими и при  *
     * их изменении не происходит автоматического перерасчёта данных для 2ого и 3го      *
     * рядов. Если изменились данные 2ого ряда, то происходит активтоматически           *
     * перерасчёт данных для 3го ряда; если изменились данные 3го ряда, то автоматически *
     * перерасчитываются данные 2ого ряда.                                               *
     *      ПРИ ИЗМЕНЕНИИ АЛГОРИТМА БУДЬ ГОТОВ ВНЕСТИ ПОПРАВКИ В ЭТОТ КОММЕНТАРИЙ!       *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    std::vector<quint16> activeIndexes;
    activeIndexes.reserve(numGases);
    for (quint16 index = 0; index < m_widgetRows.size(); index++)
        if (m_widgetRows[index].isActive())
            activeIndexes.push_back(index);
    auto activeCount = activeIndexes.size();

    // Имеется 1 активный ряд виджетов
    if (activeCount == 1)
    {
        // Индекс активного ряда и изменяемого совпадают
        if (activeIndexes[0] == indexChanged)
            m_widgetRows[indexChanged].setMoleFrac(100.0f);
        // Ряд виджетов с индексом indexChanged был отключён
        else
        {
            m_widgetRows[activeIndexes[0]].setMoleFrac(100.0f);
            m_widgetRows[indexChanged].m_moleFracInput->setValue(0);
        }
    }
    // Имеется 2 активных ряда виджетов
    else if (activeCount == 2)
    {
        auto &first = m_widgetRows[activeIndexes[0]];
        auto &second = m_widgetRows[activeIndexes[1]];
        if (activeIndexes[0] == indexChanged)
            second.setMoleFrac(100.0f - first.getMoleFrac());
        else if (activeIndexes[1] == indexChanged)
            first.setMoleFrac(100.0f - second.getMoleFrac());
        // Ряд виджетов с индексом indexChanged был отключён
        else
        {
            m_widgetRows[indexChanged].m_moleFracInput->setValue(0);
            second.setMoleFrac(100.0f - first.getMoleFrac());
        }
    }
    // Имеется 3 активных ряда виджетов
    else if (activeCount == 3)
    {
        // Данные изменились в 1ом ряде
        if (indexChanged == 0)
        {
            // ignore, нет автоматического перерасчёта
        }
        // Данные изменились во 2ом ряде, перерасчёт 3го активного ряда виджетов
        else if (indexChanged == 1)
        {
            auto recalc = 100.0f - (m_widgetRows[0].getMoleFrac() + m_widgetRows[1].getMoleFrac());
            m_widgetRows[2].setMoleFrac(recalc);
        }
        // Данные изменились во 3ем ряде, перерасчёт 2ого активного ряда виджетов
        else if (indexChanged == 2)
        {
            auto recalc = 100.0f - (m_widgetRows[0].getMoleFrac() + m_widgetRows[2].getMoleFrac());
            m_widgetRows[1].setMoleFrac(recalc);
        }
        // Данные изменились в несуществующем ряде виджетов (???)
        else
            assert(false && "Unknown widget");
    }

    setStatus(checkValues());
}

void GasDensityWidget::fill(const S2::CONF_DENS_3t &value)
{
    for (auto index = 0; index < m_widgetRows.size(); index++)
    {
        const auto &elem = value[index];
        auto &widgetRow = m_widgetRows[index];
        widgetRow.setGasType(GasType(elem.TypeGaz));
        widgetRow.setWeight(elem.Weight);
        widgetRow.setMolarMass(elem.MolW);
        widgetRow.setMoleFrac(elem.MolFrac);
    }
}

S2::CONF_DENS_3t GasDensityWidget::fillBack() const
{
    S2::CONF_DENS_3t retData;
    for (auto index = 0; index < m_widgetRows.size(); index++)
    {
        auto &elem = retData[index];
        const auto &widgetRow = m_widgetRows[index];
        elem.TypeGaz = std_ext::to_underlying(widgetRow.getGasType());
        elem.Weight = widgetRow.getWeight();
        elem.MolW = widgetRow.getMolarMass();
        elem.MolFrac = widgetRow.getMoleFrac();
    }
    return retData;
}

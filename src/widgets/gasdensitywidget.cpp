#include "gasdensitywidget.h"

#include <QComboBox>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <gen/std_ext.h>

constexpr float one_hundred = 100.0f;

bool GasWidgetRow::isActive() const noexcept
{
    return (weightInput->isEnabled() || moleFracInput->isEnabled());
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
        weightInput->setEnabled(false);
        moleFracInput->setEnabled(false);
    }
    else
    {
        if (gasType == GasType::Other)
            molarMassInput->setEnabled(true);
        else
            molarMassInput->setEnabled(false);
        if (inputMode == InputMode::InputMass)
            weightInput->setEnabled(true);
        else
            weightInput->setEnabled(false);
        if (inputMode == InputMode::InputMoleFrac)
            moleFracInput->setEnabled(true);
        else
            moleFracInput->setEnabled(false);
    }
}

GasType GasWidgetRow::getGasType() const noexcept
{
    // Костыль :)
    auto index = gasTypeInput->currentIndex();
    constexpr int otherIndex = std_ext::to_underlying(GasType::HydrogenFluoride) + 1;
    if (index == otherIndex)
        return GasType::Other;
    else
        return GasType(index);
}

float GasWidgetRow::getMolarMass() const noexcept
{
    return getLineEditData(molarMassInput);
}

float GasWidgetRow::getWeight() const noexcept
{
    return getLineEditData(weightInput);
}

float GasWidgetRow::getMoleFrac() const noexcept
{
    return getLineEditData(moleFracInput);
}

float GasWidgetRow::getMoles() const noexcept
{
    const auto molarMass = getMolarMass();
    const auto weight = getWeight();
    constexpr float kilo = 1000;
    if (molarMass != 0)
        return (weight * 1000) / molarMass;
    else
        return 0;
}

void GasWidgetRow::setGasType(const GasType type) noexcept
{
    // Костыль :)
    quint32 index = 0;
    if (type == GasType::Other)
        index = std_ext::to_underlying(GasType::HydrogenFluoride) + 1;
    else
        index = std_ext::to_underlying(type);
    gasTypeInput->setCurrentIndex(index);
}

void GasWidgetRow::setMolarMass(const float molarMass) noexcept
{
    setLineEditData(molarMass, molarMassInput);
}

void GasWidgetRow::setWeight(const float mass) noexcept
{
    setLineEditData(mass, weightInput);
}

void GasWidgetRow::setMoleFrac(const float moleFrac) noexcept
{
    setLineEditData(moleFrac, moleFracInput);
}

void GasWidgetRow::setTotalMoles(const float totalMoles) noexcept
{
    const auto moles = getMoles();
    const auto moleFrac = (moles / totalMoles) * one_hundred;
    setMoleFrac(moleFrac);
}

GasDensityWidget::GasDensityWidget(QWidget *parent)
    : QWidget(parent), workMode(InputMode::InputMass), status(Status::Correct), statusWidget(new QLabel("", this))
{
    setupUI();
}

QLineEdit *GasDensityWidget::createLineEdit(float min, float max, int decimals, const QString &text)
{
    auto lineEdit = new QLineEdit(text, this);
    auto validator = new QDoubleValidator(this);
    validator->setNotation(QDoubleValidator::Notation::StandardNotation);
    validator->setRange(min, max, decimals);
    lineEdit->setValidator(validator);
    return lineEdit;
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

QLineEdit *GasDensityWidget::createMolarMassWidget()
{
    auto molarMassWidget = createLineEdit(0, 500);
    connect(molarMassWidget, &QLineEdit::textEdited, this, //
        [this](const auto &) {
            if (workMode == InputMode::InputMass)
                recalc();
        });
    return molarMassWidget;
}

QLineEdit *GasDensityWidget::createMassWidget()
{
    auto massWidget = createLineEdit(0, 100);
    connect(massWidget, &QLineEdit::textEdited, this, //
        [this](const auto &) { recalc(); });
    return massWidget;
}

QLineEdit *GasDensityWidget::createMoleFracWidget(std::size_t index)
{
    auto moleFracWidget = createLineEdit(0, 100);
    connect(moleFracWidget, &QLineEdit::textEdited, this, //
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
        widgetRow.weightInput = createMassWidget();
        widgetRow.weightInput->setEnabled(false);
        layout->addWidget(widgetRow.weightInput, row, column++, Qt::AlignCenter);
        widgetRow.moleFracInput = createMoleFracWidget(index);
        widgetRow.moleFracInput->setEnabled(false);
        layout->addWidget(widgetRow.moleFracInput, row, column++, Qt::AlignCenter);
        ++index;
    }
    setStatus(Status::Incorrect);
    layout->addWidget(statusWidget, ++row, 0, 1, 5, Qt::AlignCenter);
    setLayout(layout);
}

void GasDensityWidget::setStatus(const Status newStatus)
{
    if (status != newStatus)
    {
        status = newStatus;
        if (status == Status::Incorrect)
        {
            statusWidget->setText("Incorrect data");
            statusWidget->setStyleSheet("QLabel { color : red; }");
        }
        else
        {
            statusWidget->setText("Correct data");
            statusWidget->setStyleSheet("QLabel { color : green; }");
        }
    }
}

Status GasDensityWidget::checkValues()
{
    Status newStatus = Status::Correct;
    float totalMoleFrac = 0;
    for (auto &widgetRow : widgetRows)
    {
        if (widgetRow.isActive())
        {
            const auto moleFrac = widgetRow.getMoleFrac();
            // Суммируем в totalMoleFrac
            totalMoleFrac += moleFrac;
            // Проверка, что каждое поле лежит в диапазоне от 0% до 100% и не является NaN
            if (moleFrac < 0.0f || moleFrac > one_hundred || std::isnan(moleFrac))
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
        constexpr float epsilon = std::numeric_limits<float>::epsilon() * one_hundred * 10;
        const float diff = std::fabs(totalMoleFrac - one_hundred);
        // qWarning() << diff << " " << epsilon;
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
        for (auto &widgetRow : widgetRows)
        {
            auto typeIndex = widgetRow.gasTypeInput->currentIndex();
            // Отключаем ввод мольной доли, только отображаем
            widgetRow.moleFracInput->setEnabled(false);
            // Включаем поля ввода массы газа, если выбран тип газа
            if (typeIndex != std_ext::to_underlying(GasType::NotChosen))
                widgetRow.weightInput->setEnabled(true);
            widgetRow.setWeight(1);
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
            widgetRow.weightInput->setEnabled(false);
            widgetRow.weightInput->setText("Нет");
            // Включаем поля ввода мольной доли газа, если выбран тип газа
            if (typeIndex != std_ext::to_underlying(GasType::NotChosen))
                widgetRow.moleFracInput->setEnabled(true);
        }
    }
    workMode = newInputMode;
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
            widgetRow.setWeight(1);
    }
    if (newGasType == GasType::NotChosen)
    {
        widgetRow.moleFracInput->setText("Нет");
        widgetRow.weightInput->setText("Нет");
    }
    if (workMode == InputMode::InputMass)
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
    for (auto &widgetRow : widgetRows)
        if (widgetRow.isActive())
            totalMoles += widgetRow.getMoles();

    for (auto &widgetRow : widgetRows)
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
    for (quint16 index = 0; index < widgetRows.size(); index++)
        if (widgetRows[index].isActive())
            activeIndexes.push_back(index);
    auto activeCount = activeIndexes.size();

    // Имеется 1 активный ряд виджетов
    if (activeCount == 1)
    {
        // Индекс активного ряда и изменяемого совпадают
        if (activeIndexes[0] == indexChanged)
            widgetRows[indexChanged].setMoleFrac(one_hundred);
        // Ряд виджетов с индексом indexChanged был отключён
        else
        {
            widgetRows[activeIndexes[0]].setMoleFrac(one_hundred);
            widgetRows[indexChanged].moleFracInput->setText("Нет");
        }
    }
    // Имеется 2 активных ряда виджетов
    else if (activeCount == 2)
    {
        auto &first = widgetRows[activeIndexes[0]];
        auto &second = widgetRows[activeIndexes[1]];
        if (activeIndexes[0] == indexChanged)
            second.setMoleFrac(one_hundred - first.getMoleFrac());
        else if (activeIndexes[1] == indexChanged)
            first.setMoleFrac(one_hundred - second.getMoleFrac());
        // Ряд виджетов с индексом indexChanged был отключён
        else
        {
            widgetRows[indexChanged].moleFracInput->setText("Нет");
            second.setMoleFrac(one_hundred - first.getMoleFrac());
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
            auto recalc = one_hundred - (widgetRows[0].getMoleFrac() + widgetRows[1].getMoleFrac());
            widgetRows[2].setMoleFrac(recalc);
        }
        // Данные изменились во 3ем ряде, перерасчёт 2ого активного ряда виджетов
        else if (indexChanged == 2)
        {
            auto recalc = one_hundred - (widgetRows[0].getMoleFrac() + widgetRows[2].getMoleFrac());
            widgetRows[1].setMoleFrac(recalc);
        }
        // Данные изменились в несуществующем ряде виджетов (???)
        else
            assert(false && "Unknown widget");
    }

    setStatus(checkValues());
}

void GasDensityWidget::fill(const DataTypes::CONF_DENS_3t &value)
{
    for (auto index = 0; index < widgetRows.size(); index++)
    {
        const auto &elem = value[index];
        auto &widgetRow = widgetRows[index];
        widgetRow.setGasType(GasType(elem.TypeGaz));
        widgetRow.setWeight(elem.Weight);
        widgetRow.setMolarMass(elem.MolW);
        widgetRow.setMoleFrac(elem.MolFrac);
    }
}

DataTypes::CONF_DENS_3t GasDensityWidget::fillBack() const
{
    DataTypes::CONF_DENS_3t retData;
    for (auto index = 0; index < widgetRows.size(); index++)
    {
        auto &elem = retData[index];
        const auto &widgetRow = widgetRows[index];
        elem.TypeGaz = std_ext::to_underlying(widgetRow.getGasType());
        elem.Weight = widgetRow.getWeight();
        elem.MolW = widgetRow.getMolarMass();
        elem.MolFrac = widgetRow.getMoleFrac();
    }
    return retData;
}

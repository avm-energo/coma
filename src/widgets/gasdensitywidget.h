#pragma once

#include <QWidget>
#include <array>
#include <gen/datatypes.h>

class QComboBox;
class QDoubleSpinBox;
class QLabel;

constexpr inline std::size_t numGases = 3;

/// \brief Перечисление типов газов для выбора пользователем.
enum class GasType : qint32
{
    NotChosen = 0,       ///< Не выбран.
    SulfurHexafluoride,  ///< Элегаз (гексафторид  серы).
    CarbonTetrafluoride, ///< Тетрафторметан (тетрафторид углерода).
    Hexafluoroethane,    ///< Гексафторэтан.
    Nitrogen,            ///< Азот.
    Oxygen,              ///< Кислород.
    Argon,               ///< Аргон.
    HydrogenFluoride,    ///< Фтороводород (фторид водорода).
    Other = 0xffff       ///< Другой газ.
};

/// \brief Перечисление, определяющее тип ввода информации в виджете.
enum class InputMode : bool
{
    InputMass = false,   ///< Режим ввода масс газов.
    InputMoleFrac = true ///< Режим ввода мольных долей газов.
};

/// \brief Перечисление, определяющее статус введённых пользователем данных.
enum class Status : bool
{
    Incorrect = false, ///< Введённая информация некорректна.
    Correct = true     ///< Введённая информация корректна.
};

/// \brief Ряд виджетов для ввода данных об одном газе.
struct GasWidgetRow
{
    QComboBox *m_gasTypeInput = nullptr;        ///< Поле "тип газа".
    QDoubleSpinBox *m_molarMassInput = nullptr; ///< Поле "молярная масса".
    QDoubleSpinBox *m_weightInput = nullptr;    ///< Поле "масса".
    QDoubleSpinBox *m_moleFracInput = nullptr;  ///< Поле "мольная доля".

    /// \brief Является ли текущий ряд активным (выбран ли тип газа).
    bool isActive() const noexcept;
    /// \brief В зависимости от указанного типа газа и текущего режима ввода данных,
    /// активирует и деактивирует виджеты для текущего ряда.
    void layoutAction(const GasType gasType, const InputMode inputMode) noexcept;

    /// \brief Возвращает выбранный тип газа.
    GasType getGasType() const noexcept;
    /// \brief Возвращает молярную массу текущего газа.
    float getMolarMass() const noexcept;
    /// \brief Возвращает массу текущего газа.
    float getWeight() const noexcept;
    /// \brief Возвращает мольную долю текущего газа.
    float getMoleFrac() const noexcept;
    /// \brief Возвращает количество молей текущего газа.
    /// \see getMolarMass, getMass
    float getMoles() const noexcept;

    /// \brief Устанавливает новый тип газа.
    void setGasType(const GasType type) noexcept;
    /// \brief Устанавливает молярную массу для текущего газа.
    void setMolarMass(const float molarMass) noexcept;
    /// \brief Устанавливает новую массу для текущего газа.
    void setWeight(const float weight) noexcept;
    /// \brief Устанавливает новую мольную долю для текущего газа.
    void setMoleFrac(const float moleFrac) noexcept;
    /// \brief Передаёт общее количество молей всех газов.
    /// \details При передаче выполняется перерасчёт мольной доли
    /// текущего газа. Используется только в режиме ввода масс газов.
    void setTotalMoles(const float totalMoles) noexcept;
};

/// \brief Виджет для задания информации о смеси газов в конфигурации устройства.
class GasDensityWidget : public QWidget
{
    Q_OBJECT
private:
    std::array<GasWidgetRow, numGases> m_widgetRows;
    InputMode m_workMode;
    Status m_status;
    QLabel *m_statusWidget;

    /// \brief Создаёт и возвращает указатель на QLineEdit для ввода чисел с плавающей точкой.
    QDoubleSpinBox *createSpinBox(float min = 0, float max = 100, int decimals = 2, float defaultValue = 0);
    /// \brief Создаёт и возвращает указатель на виджет для выбора типа газа.
    QComboBox *createGasTypeWidget(std::size_t index);
    /// \brief Создаёт и возвращает указатель на виджет для ввода молярной массы.
    QDoubleSpinBox *createMolarMassWidget();
    /// \brief Создаёт и возвращает указатель на виджет для ввода массы газа.
    QDoubleSpinBox *createMassWidget();
    /// \brief Создаёт и возвращает указатель на виджет для ввода мольной доли газа.
    QDoubleSpinBox *createMoleFracWidget(const size_t index);
    /// \brief Создаёт и настраивает пользовательский интерфейс для виджета.
    void setupUI();

    /// \brief Устанавливает новый статус для введённых пользователем данных.
    void setStatus(const Status newStatus);
    /// \brief Осуществляет валидацию введённых в виджет данных.
    Status checkValues();

    /// \brief Вызывается при изменении типа ввода данных в виджет.
    void inputModeChanged(const InputMode newInputMode);
    /// \brief Вызывается при изменении типа газа у конкретного ряда виджетов.
    void gasTypeChanged(const std::size_t index, const GasType newGasType);

    /// \brief Перерасчёт данных при режиме ввода масс газов.
    void recalc();
    /// \brief Перерасчёт данных при режиме ввода мольных долей газов.
    void recalc(const std::size_t indexChanged);

public:
    explicit GasDensityWidget(QWidget *parent = nullptr);

    /// \brief Заполнить виджет данными в формате S2.
    void fill(const DataTypes::CONF_DENS_3t &value);
    /// \brief Получить данные от виджета в формате S2.
    DataTypes::CONF_DENS_3t fillBack() const;
};

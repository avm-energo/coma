#pragma once

#include <QWidget>
#include <array>

class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QLabel;

constexpr inline std::size_t numGases = 3;

/// \brief Перечисление типов газов для выбора пользователем.
enum class GasType : quint8
{
    NotChosen = 0,       ///< Не выбран.
    SulfurHexafluoride,  ///< Элегаз (гексафторид  серы).
    CarbonTetrafluoride, ///< Тетрафторметан (тетрафторид углерода).
    Hexafluoroethane,    ///< Гексафторэтан.
    Nitrogen,            ///< Азот.
    Oxygen,              ///< Кислород.
    Argon,               ///< Аргон.
    HydrogenFluoride,    ///< Фтороводород (фторид водорда).
    Other = 0xff         ///< Другой газ.
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
    QComboBox *gasTypeInput = nullptr;   ///< Поле "тип газа".
    QLineEdit *molarMassInput = nullptr; ///< Поле "молярная масса".
    QLineEdit *massInput = nullptr;      ///< Поле "масса".
    QLineEdit *moleFracInput = nullptr;  ///< Поле "мольная доля".

    /// \brief Является ли текущий ряд активным (выбран ли тип газа).
    bool isActive() const noexcept;
    /// \brief Возвращает значение с плавающей точкой из содержимого QLineEdit.
    float getLineEditData(const QLineEdit *lineEdit) const noexcept;
    /// \brief Устанавливает значение с плавающей точкой в качестве содержимого QLineEdit.
    /// \param data - усьанавливаемое значение с плавающей точкой.
    /// \param lineEdit - QLineEdit, содержимое которого будет изменено.
    void setLineEditData(const float data, QLineEdit *lineEdit) noexcept;
    /// \brief В зависимости от указанного типа газа и текущего режима ввода данных,
    /// активирует и деактивирует виджеты для текущего ряда.
    void layoutAction(const GasType gasType, const InputMode inputMode) noexcept;

    /// \brief Возвращает молярную массу текущего газа.
    float getMolarMass() const noexcept;
    /// \brief Возвращает массу текущего газа.
    float getMass() const noexcept;
    /// \brief Возвращает мольную долю текущего газа.
    float getMoleFrac() const noexcept;
    /// \brief Возвращает количество молей текущего газа.
    /// \details
    /// \see getMolarMass, getMass
    float getMoles() const noexcept;

    /// \brief Устанавливает молярную массу для текущего газа.
    void setMolarMass(const float molarMass) noexcept;
    /// \brief Устанавливает новую массу для текущего газа.
    void setMass(const float mass) noexcept;
    /// \brief Устанавливает новую мольную долю для текущего газа.
    void setMoleFrac(const float moleFrac) noexcept;
    /// \brief Передаёт общее количество молей всех газов.
    /// \details При передаче выполняется перерасчёт мольной доли
    /// текущего газа. Используется только в режиме ввода масс газов.
    void setTotalMoles(const float totalMoles) noexcept;
};

/// \brief Виджет для задания информации о смеси газов в конфигурации устройства.
class GasCompositionWidget : public QWidget
{
    Q_OBJECT
private:
    std::array<GasWidgetRow, numGases> widgetRows;
    InputMode workMode;
    Status status;
    QLabel *statusWidget;

    /// \brief Создаёт и возвращает указатель на QLineEdit для ввода чисел с плавающей точкой.
    QLineEdit *createLineEdit(float min = 0, float max = 100, int decimals = 10, const QString &text = "0,0");
    /// \brief Создаёт и возвращает указатель на виджет для выбора типа газа.
    QComboBox *createGasTypeWidget(std::size_t index);
    /// \brief Создаёт и возвращает указатель на виджет для ввода молярной массы.
    QLineEdit *createMolarMassWidget();
    /// \brief Создаёт и возвращает указатель на виджет для ввода массы газа.
    QLineEdit *createMassWidget();
    /// \brief Создаёт и возвращает указатель на виджет для ввода мольной доли газа.
    QLineEdit *createMoleFracWidget(std::size_t index);
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

    /// \brief Перерассчёт данных при режиме ввода масс газов.
    void recalc();
    /// \brief Перерассчёт данных при режиме ввода мольных долей газов.
    void recalc(const std::size_t indexChanged);

public:
    explicit GasCompositionWidget(QWidget *parent = nullptr);
    // void fill();
    // void fillBack();
};

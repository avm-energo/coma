#pragma once

#include <QWidget>
#include <array>

class QComboBox;
class QDoubleSpinBox;
class QLineEdit;

constexpr static std::size_t numGases = 3;

enum class GasType : quint8
{
    NotChosen = 0,       ///< Не выбран
    SulfurHexafluoride,  ///< Элегаз (гексафторид  серы)
    CarbonTetrafluoride, ///< Тетрафторметан (тетрафторид углерода)
    Hexafluoroethane,    ///< Гексафторэтан
    Nitrogen,            ///< Азот
    Oxygen,              ///< Кислород
    Argon,               ///< Аргон
    HydrogenFluoride,    ///< Фтороводород (фторид водорда)
    Other = 0xff         ///< Другой газ
};

enum class InputMode : bool
{
    InputMass = false,
    InputMoleFrac = true
};

struct GasWidgetRow
{
    QComboBox *gasTypeInput = nullptr;   ///< Поле "тип газа"
    QLineEdit *molarMassInput = nullptr; ///< Поле "молярная масса"
    QLineEdit *massInput = nullptr;      ///< Поле "масса"
    QLineEdit *moleFracInput = nullptr;  ///< Поле "мольная доля"

    bool isActive() const noexcept;
    float getLineEditData(const QLineEdit *lineEdit) const noexcept;
    void setLineEditData(const float data, QLineEdit *lineEdit) noexcept;
    void layoutAction(const GasType gasType, const InputMode inputMode) noexcept;

    float getMolarMass() const noexcept;
    float getMass() const noexcept;
    float getMoleFrac() const noexcept;
    float getMoles() const noexcept;
    void setMolarMass(const float molarMass) noexcept;
    void setMass(const float mass) noexcept;
    void setMoleFrac(const float moleFrac) noexcept;
    void setTotalMoles(const float totalMoles) noexcept;
};

class GasCompositionWidget : public QWidget
{
    Q_OBJECT
private:
    std::array<GasWidgetRow, numGases> widgetRows;
    InputMode workMode;

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

private slots:
    void inputModeChanged(const InputMode newInputMode);
    /// \brief Вызывается при изменении типа газа у конкретного ряда виджетов.
    void gasTypeChanged(const std::size_t index, const GasType newGasType);
    /// \brief Пересчитывает текущее содержимое виджетов.
    void recalc();
    void moleFracChanged(const std::size_t index, const float newMoleFrac);

public:
    explicit GasCompositionWidget(QWidget *parent = nullptr);
    // void fill();
    // void fillBack();
};

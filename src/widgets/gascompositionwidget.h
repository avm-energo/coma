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

struct GasWidgetRow
{
    QComboBox *gasTypeInput = nullptr;   ///< Поле "тип газа"
    QLineEdit *molarMassInput = nullptr; ///< Поле "молярная масса"
    QLineEdit *massInput = nullptr;      ///< Поле "масса"
    QLineEdit *moleFracInput = nullptr;  ///< Поле "мольная доля"
};

class GasCompositionWidget : public QWidget
{
    Q_OBJECT
private:
    std::array<GasWidgetRow, numGases> widgetRows;

    /// \brief TODO
    QLineEdit *createLineEdit(float min = 0.0f, float max = 100.0f, int decimals = 10, const QString &text = "0,0");
    /// \brief Создаёт и возвращает указатель на виджет для выбора типа газа.
    QComboBox *createGasTypeWidget(std::size_t index);
    /// \brief Создаёт и возвращает указатель на виджет для ввода молярной массы.
    QLineEdit *createMolarMassWidget(std::size_t index);
    /// \brief Создаёт и возвращает указатель на виджет для ввода массы газа.
    QLineEdit *createMassWidget(std::size_t index);
    /// \brief Создаёт и возвращает указатель на виджет для ввода мольной доли газа.
    QLineEdit *createMoleFracWidget(std::size_t index);
    /// \brief Создаёт и настраивает пользовательский интерфейс для виджета
    void setupUI();

private slots:
    void gasTypeChanged(std::size_t index, GasType newGasType);
    void molarMassChanged(std::size_t index, float newMolarMass);
    void massChanged(std::size_t index, float newMass);
    void moleFracChanged(std::size_t index, float newMoleFrac);

public:
    explicit GasCompositionWidget(QWidget *parent = nullptr);
    // void fill();
    // void fillBack();
};

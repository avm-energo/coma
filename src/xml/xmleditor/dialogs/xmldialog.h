#pragma once

#include <QComboBox>
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <variant>

constexpr auto createId = -1;
constexpr auto idMin = 1;
constexpr auto idMax = 0xffff;
constexpr auto addrMin = idMin;
constexpr auto addrMax = 0xffffff;
constexpr auto configCountMin = 0;
constexpr auto configCountMax = 0xff;
constexpr auto countMin = addrMin;
constexpr auto countMax = configCountMax;

/// \brief Базовый абстрактный класс для создания диалоговых окон XML редактора.
class XmlDialog : public QDialog
{
    Q_OBJECT
private:
    using Widget = std::variant<QLineEdit *, QComboBox *, QSpinBox *>;

protected:
    QString mTitle;
    bool isChanged;
    int mRow;
    QList<Widget> dlgItems;

    /// \brief Функция устанавливает фиксированный размер окна и отображает его в центре экрана.
    void setupSizePos(int width, int height);

    /// \brief Функция принимает главный слой, добавляет туда кнопку для сохранения данных.
    void addSaveBtnAndApply(QVBoxLayout *mainLayout);

    /// \brief Виртуальный метод для создания UI диалога (виджетов).
    virtual void setupUI(QVBoxLayout *mainLayout) = 0;

    /// \brief Виртуальный метод для сбора информации с виджетов при сохранении данных.
    virtual QStringList collectData();

    /// \brief Виртуальный метод для проверки на корректность введённых данных.
    virtual bool checkDataBeforeSaving(const QStringList &savedData);

public:
    explicit XmlDialog() = delete;
    explicit XmlDialog(QWidget *parent = nullptr);

    /// \brief Функция для вызова виртуального метода setupUI.
    /// \details Вызывает setupUI, в качестве параметров передаётся строка с данными из модели.
    void startup(int row = createId);

    /// \brief Функция, вызываемая при закрытии диалогового окна.
    virtual void reject() override;

signals:
    /// \brief Сигнал-запрос к модели за данными, содержащимися по указанной строке.
    void modelDataRequest(const int &row);

    /// \brief Сигнал для отправки данных модели для создания нового элемента.
    void createData(const QStringList &saved, int *rowPtr);

    /// \brief Сигнал для отправки данных модели для редактирования указанного элемента.
    void updateData(const QStringList &saved, const int &row);

public slots:
    /// \brief Виртуальный слот, вызывается когда диалог получает данные от модели.
    /// \details Здесь описывается вся логика раскидывания данных от модели по виджетам.
    virtual void modelDataResponse(const QStringList &response);

    /// \brief Виртуальная функция, вызываемая при сохранении данных.
    virtual void saveData();

    /// \brief Слот, вызываемый при изменении данных в диалоговом окне.
    void dataChanged();

    /// \overload dataChanged.
    /// \brief Перегрузка слота, вызываемого при изменении данных в диалоговом окне.
    /// \see dataChanged.
    void dataChanged(const QString &strData);

    /// \overload dataChanged.
    /// \brief Перегрузка слота, вызываемого при изменении данных в диалоговом окне.
    /// \see dataChanged.
    void dataChanged(int data);

    /// \brief Вызывается, когда приходящие от модели данные изменили состояние
    /// диалогового окна, однако нам не нужно при создании окна изменения состояния change.
    /// \see dataChanged.
    void resetChangeState();
};

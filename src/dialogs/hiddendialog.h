#pragma once

#include "../widgets/udialog.h"

#include <device/xml_settings.h>
#include <map>
#include <set>

class QBoxLayout;
class QGroupBox;
class QPaintEvent;
class QTabWidget;

/// \brief Класс диалога "Секретные операции"
class HiddenDialog final : public UDialog
{
    Q_OBJECT
private:
    Device::XmlDataTypes::HiddenSettings m_settings;
    std::map<quint32, bool> m_srcAddrStates;
    QString m_currentBackground;
    DataTypes::HardwareStruct m_hardwareInfo;
    bool m_isGodMode, m_isAlreadyFilled;
    bool m_isSendedEnableCmd, m_isSendedWritingCmd;

    /// \brief Функция для генерации настроек диалога по умолчанию.
    /// \ingroup Data manipulations
    void generateDefaultSettings();
    /// \brief Функция для подготовки внутренних данных диалога
    /// и настройки запросов диалога к устройству.
    /// \ingroup Data manipulations
    void prepareInternalData(const Device::XmlDataTypes::SignalMap &sigMap);

    /// \brief Создание и настройка UI диалога.
    /// \ingroup UI
    void setupUI();
    /// \brief Возвращает group box, созданный по переданным настройкам вкладки.
    /// \ingroup UI
    QGroupBox *setupGroupBox(const Device::XmlDataTypes::HiddenTab &hiddenTab);
    /// \brief Перегруженная функция для отрисовки фонового изображения.
    /// \ingroup UI
    void paintEvent(QPaintEvent *e) override;

    /// \brief Обновление UI, в зависимости от того, включена ли вкладка с виджетами.
    /// \see isTabEnabled, updateWidget.
    /// \ingroup Internal logic
    void updateUI();
    /// \brief Функция для проверки, включена ли вкладка с виджетами.
    /// \details Проверка происходит с помощью чек-бокса, отображаемого на всех
    /// вкладках, кроме вкалдки для базовой платы (она всегда включена по умолчанию).
    /// \ingroup Internal logic
    bool isTabEnabled(const Device::XmlDataTypes::HiddenTab &tabSettings) const noexcept;
    /// \brief Обновление (enabling) виджета на вкладке.
    /// \ingroup Internal logic
    void updateWidget(const bool enabled, const Device::XmlDataTypes::HiddenWidget &widget);

    /// \brief Функция для поиска виджета в диалоге по его адресу.
    /// \ingroup Internal logic
    const Device::XmlDataTypes::HiddenWidget *findWidgetByAddress(const quint32 addr) const noexcept;
    /// \brief Функция выполняет проверку, получили ли все виджеты
    /// диалога данные при выполнении запроса к устройству.
    /// \ingroup Internal logic
    void verifyFilling() noexcept;
    /// \brief Функция для принятия данных в формате BitString, запрошенных у устройства.
    /// \ingroup Internal logic
    void updateBitStringData(const DataTypes::BitStringStruct &bs) override;
    /// \brief Функция для заполнения виджета принятыми от устройства данными.
    /// \ingroup Internal logic
    void fillWidget(const quint32 value, const Device::XmlDataTypes::HiddenWidget &widgetData);

    /// \brief Функция для парсинга данных из виджета.
    /// \ingroup Internal logic
    quint32 getDataFrom(const Device::XmlDataTypes::HiddenWidget &widgetData);
    /// \brief Функция для принятия от устройства данных в формате GeneralResponse.
    /// \details Запись данных в устройство делится на 2 этапа:
    /// - отправление команды EnableWritingHardware;
    /// - если на прошлую команду от устройство пришёл статус Ok,
    /// то блок с данными отправляется в устройство.
    /// \ingroup Internal logic
    void updateGeneralResponse(const DataTypes::GeneralResponseStruct &response) override;

public:
    explicit HiddenDialog(Device::CurrentDevice *device, QWidget *parent = nullptr);
    /// \brief Устанавливает имя устройства в диалоге.
    /// \ingroup API
    void setModuleName(const QString &moduleName);

public slots:
    /// \brief Слот для отправки запроса устройству и заполнения
    /// виджетов диалога данными из пришедшего ответа.
    /// \ingroup API
    void fill();
    /// \brief Слот для записи данных диалога в устройство.
    /// \ingroup API
    void write();
};

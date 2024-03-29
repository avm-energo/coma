#pragma once

#include <alarms/basealarm.h>
#include <bitset>

constexpr quint32 BSIWARNMASK = 0x00005F55;  ///< Warning alarms bit mask.
constexpr quint32 BSIALARMMASK = 0x000020AA; ///< Critical alarms bit mask.

namespace AVM
{

const QStringList Health {
    "ADCI", //
    "FLS",  //
    "TUP",  //
    "ADCB", //
    "1PPS", //
    "ADCM", //
    "REGB", //
    "RCN",  //
    "HWIB", //
    "HWIM", //
    "REGM", //
    "BAT",  //
    "NTP",  //
    "FLS2", //
    "FRM"   //
};

const QStringList HthToolTip {
    "Проблемы со встроенным АЦП ",                                                      //
    "Не работает внешняя flash-память",                                                 //
    "Перегрев",                                                                         //
    "Проблемы с АЦП (нет связи) (базовая)",                                             //
    "Нет сигнала 1PPS с антенны",                                                       //
    "Проблемы с АЦП (нет связи) (мезонин)",                                             //
    "Ошибка регулировочных коэффициентов (базовая)",                                    //
    "Ошибка загрузки конфигурации из flash-памяти. Работает конфигурация по умолчанию", //
    "Некорректная Hardware информация (базовая)",                                       //
    "Некорректная Hardware информация (мезонин)",                                       //
    "Ошибка регулировочных коэффициентов (мезонин)",                                    //
    "Напряжение батареи низко (< 2,5 В)",                                               //
    "Нет связи с NTP-сервером",                                                         //
    "Не работает внешняя flash-память (мезонин)",                                       //
    "Не работает внешняя fram",                                                         //
    "Проблемы с RTC "                                                                   //
};

}

/// \brief The class for alarm dialog, that displaying the device general status.
class AlarmStateAll : public BaseAlarm
{
    Q_OBJECT
private:
    std::bitset<128> m_alarmFlags; // '1' equals alarm

public:
    explicit AlarmStateAll(Device::CurrentDevice *device, QWidget *parent = nullptr);
    /// \brief Setup UI: creating text labels and indicators (pixmaps) for alarms displaying.
    virtual void setupUI(const QStringList &events) override;
    /// \brief Request for updating a BSI data.
    virtual void reqUpdate() override;

public slots:
    /// \brief The slot called when a device health changed for updating alarms.
    void update(quint32 health);
};

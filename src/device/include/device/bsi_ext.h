#pragma once

#include <device/xml_settings.h>

namespace Device
{

/// \brief Структура, описывающая расширенный блок базовой информации
///  (BSI Extended) для подключенного устройства.
class BlockStartupInfoExtended final : public QObject
{
    Q_OBJECT
private:
    std::map<u32, u32> m_data;
    u32 m_startReg;
    u32 m_endReg;

public:
    /// \brief Константный итератор на хранимые данные.
    using Iter = decltype(m_data)::const_iterator;

    explicit BlockStartupInfoExtended(QObject *parent = nullptr);

    /// \brief Обновляет внутреннюю структуру блока информации в соответствии
    /// с настройками из XML конфигурации подключенного устройства.
    void updateStructure(const XmlDataTypes::BsiExtItemList &items);
    /// \brief Обновляет хранимые данные блока информации.
    /// \details Единственный метод для обновления хранимой информации блока.
    void updateData(const DataTypes::BitStringStruct &value);

    /// \brief Возвращает константный итератор на начало хранимых данных.
    /// \details Константный итератор используется для того, чтобы
    /// клиентский код мог только читать информацию из блока, но не менять её.
    [[nodiscard]] Iter begin() const noexcept;
    /// \brief Возвращает константный итератор на конец хранимых данных.
    /// \details Константный итератор используется для того, чтобы
    /// клиентский код мог только читать информацию из блока, но не менять её.
    [[nodiscard]] Iter end() const noexcept;
    /// \brief Возвращает хранимой значение по указанному адресу.
    /// \details Если указанный адрес выходит за диапазоны хранимых
    /// данных, то будет возвращено значение по умолчанию.
    [[nodiscard]] u32 operator[](const u32 address) const;

signals:
    /// \brief Сигнал для уведомления о том, что хранимые данные были обновлены.
    void wasUpdated();
};

} // namespace Device

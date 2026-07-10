#pragma once

#include <device/xml_settings.h>

#include <vector>

namespace Device
{

constexpr quint16 bsiExtStartReg = 40;

/// \brief Расширенный блок базовой информации (BSI Extended) — вектор u32
///        переменной длины, индексируемый через поле <offset> из <bsi-ext>.
class BlockStartupInfoExtended final : public QObject
{
    Q_OBJECT
private:
    std::vector<u32> m_data; ///< значения по offset-индексам из XML
    u32 m_startReg;          ///< первый регистр устройства для запроса
    u32 m_endReg;            ///< последний регистр устройства для запроса

public:
    using Iter = std::vector<u32>::const_iterator;

    explicit BlockStartupInfoExtended(QObject *parent = nullptr);

    /// \brief Настраивает размер вектора и адресный диапазон по записям <bsi-ext>.
    void updateStructure(const XmlDataTypes::BsiExtSettings &records);
    /// \brief Обновляет хранимые данные по адресу из BitStringStruct.
    void updateData(const DataTypes::BitStringStruct &value);

    [[nodiscard]] Iter begin() const noexcept;
    [[nodiscard]] Iter end() const noexcept;
    /// \brief Возвращает значение по offset-индексу (из тега <offset> в XML).
    [[nodiscard]] u32 operator[](u32 offset) const;
    [[nodiscard]] u32 size() const;
    [[nodiscard]] QByteArray toByteArray();
    void fromByteArray(const QByteArray &ba);
    u32 startAddr();
    u32 endAddr();

signals:
    void wasUpdated(u32 addr);
};

} // namespace Device

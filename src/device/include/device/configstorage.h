#pragma once

#include <device/xml_settings.h>

namespace Device
{

/// \brief Class for storaging XML settings for a selected device in ConnectionDialog.
class ConfigStorage final : public QObject
{
    Q_OBJECT
private:
    XmlSettings m_settings;
    ProtocolDescription m_protocol;

public:
    /// \brief Default c-tor.
    explicit ConfigStorage(QObject *parent = nullptr);

    /// \brief Constant getter for module settings.
    [[nodiscard]] const XmlSettings &getDeviceSettings() const noexcept;
    /// \brief Constant getter for protocol description.
    [[nodiscard]] const ProtocolDescription &getProtocolDescription() const noexcept;
    /// \brief Cleaning connected device's settings.
    void clear() noexcept;

public slots:
    /// \brief Slot for saving device's signal records.
    void signalDataReceive(const u32 id, const u32 addr, const u16 count, const XmlDataTypes::SignalType type);
    /// \brief Slot for saving check's tab records.
    void tabDataReceive(const u32 id, const QString &name);
    /// \brief Slot for saving check's section records.
    void sectionDataReceive(const XmlDataTypes::SGMap &sgmap, const QString &secHead);
    /// \brief Slot for saving device's state alarm records.
    void alarmStateAllDataReceive(const XmlDataTypes::AlarmType type, const u32 index, const QString &desc);
    /// \brief Slot for saving device's alarm records.
    void alarmDataReceive(const XmlDataTypes::AlarmType type, const u32 addr, //
        const QString &desc, const QList<u32> &highlights);
    /// \brief Slot for saving module a work journal's record.
    void workJourDataReceive(const u32 id, const QString &desc);
    /// \brief Slot for saving module a measurement journal's record.
    void measJourDataReceive(const u32 index, const QString &header, //
        const XmlDataTypes::BinaryType type, bool visib);
    /// \brief Slot for saving hidden dialog's tab settings.
    void hiddenTabDataReceive(const XmlDataTypes::HiddenTab &hiddenTab);
    /// \brief Slot for saving module's BSI Ext elements.
    void bsiExtItemDataReceive(const u32 addr, const XmlDataTypes::BinaryType type, bool visib, const QString &desc);
    /// \brief Slot for saving module's protocol groups.
    void protocolGroupReceive(const Protocol::AbstractGroup &group);
};

} // namespace Device

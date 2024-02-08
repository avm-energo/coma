#pragma once

#include <device/modulesettings.h>
#include <gen/singleton.h>
#include <s2/delegate_common.h>
#include <s2/s2datafactory.h>

/// \brief Class for storaging XML settings for a selected device in ConnectionDialog.
class ConfigStorage final : public QObject
{
    Q_OBJECT
private:
    ModuleSettings m_settings;
    ProtocolDescription m_protocol;

public:
    /// \brief Default c-tor.
    explicit ConfigStorage(QObject *parent = nullptr);

    /// \brief Constant getter for module settings.
    [[nodiscard]] const ModuleSettings &getModuleSettings() const noexcept;
    /// \brief Constant getter for protocol description.
    [[nodiscard]] const ProtocolDescription &getProtocolDescription() const noexcept;
    /// \brief Cleaning connected device's settings.
    void clear() noexcept;

public slots:
    /// \brief Slot for saving device's signal records.
    void signalDataReceive(const quint32 id, const quint32 addr, //
        const quint16 count, const ModuleTypes::SignalType sigType);
    /// \brief Slot for saving check's tab records.
    void tabDataReceive(const quint32 id, const QString &name);
    /// \brief Slot for saving check's section records.
    void sectionDataReceive(const ModuleTypes::SGMap &sgmap, const QString &secHead);
    /// \brief Slot for saving device's alarm records.
    void alarmDataReceive(const ModuleTypes::AlarmType type, const quint32 addr, //
        const QString &desc, const QList<quint32> &highlights);
    /// \brief Slot for saving module a work journal's record.
    void workJourDataReceive(const quint32 id, const QString &desc);
    /// \brief Slot for saving module a measurement journal's record.
    void measJourDataReceive(const quint32 index, const QString &header, //
        const ModuleTypes::BinaryType type, bool visib);
    /// \brief Slot for saving hidden dialog's tab settings.
    void hiddenTabDataReceived(const ModuleTypes::HiddenTab &hiddenTab);
    /// \brief Slot for saving module's protocol groups
    void protocolGroupReceived(const Protocol::AbstractGroup &group);
};

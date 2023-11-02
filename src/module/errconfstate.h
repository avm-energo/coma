#pragma once

#include "../widgets/uwidget.h"

#include <QTimer>
#include <gen/stdfunc.h>

constexpr quint32 errConfAddr = 13000; ///< Default start address for ErrConfState struct access in devices.
constexpr quint32 errConfSize = 8;     ///< Default ErrConfState struct's size.

/// \brief Enumeration for configuration error types.
enum class ErrConfParam : quint32
{
    ConfParseCode = errConfAddr,
    InvalidConfId,
    SendedLength,
    RequiredLength,
    ResultWritingConf,
    SendedCRC,
    RequiredCRC,
    HardwareErrorCode
};

/// \brief Class for displaying ErrConfState struct from the device.
class ErrConfState : public UWidget
{
    Q_OBJECT
private:
    UniquePointer<QTimer> m_requestTimer;

    /// \brief Creates UI for this dialog window.
    void setupUI();
    /// \brief Configuring the request timer and the request object.
    void configureRequest();
    /// \brief Slot for updating widgets in the window with data received from the device.
    virtual void updateBitStringData(const DataTypes::BitStringStruct &bs) override;
    /// \brief Short helper function for converting incoming number to the HEX string.
    QString createHexString(quint32 value) const;
    /// \brief Slot for disabling updates from the device.
    /// \details This slot will be calling when user closes window.
    virtual void closeEvent(QCloseEvent *event) override;

public:
    explicit ErrConfState(QWidget *parent = nullptr);
    /// \brief Function for showing this dialog window.
    void show();
};

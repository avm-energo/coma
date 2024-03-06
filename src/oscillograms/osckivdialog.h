#pragma once

#include "../widgets/udialog.h"

class QLabel;
class QPushButton;
class QTimer;

/// \brief Класс для отправки команд старта осциллограмм и их чтения в АВМ-КИВ.
class OscKivDialog final : public UDialog
{
private:
    /// \brief Команда записи осциллограммы.
    struct TypeOsc
    {
        u8 n_point;
        u8 phase;
        u16 reserved;
    };

    /// \brief Перечисление для текущего состояния диалога.
    enum class State : u8
    {
        Init,
        WritingTypeOsc,
        RequestOscState,
        RequestFile,
    };

    QPushButton *m_commandBtn, *m_readBtn;
    QLabel *m_oscFilenumLbl;
    QTimer *m_reqStateOscTimer;
    u32 m_oscFilenum, m_oldOscFilenum;
    State m_state;

    void setupUI();
    void updateOscFilenum(const u32 newOscFilenum);
    void enableButtons(const bool enabling);

    void reqOscState();
    void updateBitStringData(const DataTypes::BitStringStruct &bs) override;
    void updateGeneralResponse(const DataTypes::GeneralResponseStruct &response) override;
    void receiveOscFile(const S2::FileStruct &file);

    void writeTypeOsc();
    void reqOscFile();

public:
    explicit OscKivDialog(Device::CurrentDevice *device, QWidget *parent = nullptr);
};

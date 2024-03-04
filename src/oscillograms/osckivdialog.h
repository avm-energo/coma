#pragma once

#include "../widgets/udialog.h"

class QLabel;
class QPushButton;
class QTimer;

/// \brief Класс для отправки команд старта осциллограмм и их чтения в АВМ-КИВ.
class OscKivDialog final : public UDialog
{
private:
    QPushButton *m_commandBtn, *m_readBtn;
    QLabel *m_oscFilenumLbl;
    QTimer *m_reqStateOscTimer;
    u32 m_oscFilenum, m_oldOscFilenum;

    void setupUI();
    void updateOscFilenum(const u32 newOscFilenum);
    void enableButtons(const bool enabling);

    void reqOscState();
    void updateBitStringData(const DataTypes::BitStringStruct &bs) override;

    void writeTypeOsc();
    void reqOscFile();

public:
    explicit OscKivDialog(Device::CurrentDevice *device, QWidget *parent = nullptr);
};

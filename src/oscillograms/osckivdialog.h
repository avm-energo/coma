#pragma once

#include "../widgets/udialog.h"

class QLabel;
class QPushButton;
class QTimer;

/// \brief Команда записи осциллограммы.
struct TypeOsc
{
    u8 n_point;
    u8 phase;
    u16 reserved;
};
Q_DECLARE_METATYPE(TypeOsc)

/// \brief Класс для отправки команд старта осциллограмм и их чтения в АВМ-КИВ.
class OscKivDialog : public UDialog
{
private:
    QPushButton *m_commandBtn, *m_readBtn;
    QLabel *m_oscFilenumLbl;
    QTimer *m_reqStateOscTimer;
    u16 m_oscFilenum, m_oldOscFilenum;

    void setupUI();
    void init();
    void updateOscFilenum(const u16 newOscFilenum);

public:
    explicit OscKivDialog(Device::CurrentDevice *device, QWidget *parent = nullptr);
};

#pragma once

#include "../../widgets/udialog.h"

#include <oscillograms/oscmanager.h>

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

    OscManager m_manager;
    std::unique_ptr<TrendViewModel> m_oscModel;
    std::vector<S2::FileStruct> m_fileBuffer;
    quint64 m_timestamp;

    QPushButton *m_commandBtn, *m_readBtn;
    QLabel *m_oscFilenumLbl;
    QTimer *m_reqStateOscTimer;
    u32 m_oscFilenum, m_oldOscFilenum;
    State m_state;

    /// \brief Создаёт UI диалога.
    void setupUI();
    /// \brief Обновление текущего номера осциллограммы.
    void updateOscFilenum(const u32 newOscFilenum);
    /// \brief Включает и выключает кнопки управления на UI.
    void enableButtons(const bool enabling);

    QString getFilename();

    void resetState();

    /// \brief Запрашивает статус записи осциллограмы.
    void reqOscState();
    /// \brief Перегрузка для получения статуса записи осциллограммы.
    void updateBitStringData(const DataTypes::BitStringStruct &bs) override;
    /// \brief Перегрузка для получения ошибок связи, etc...
    void updateGeneralResponse(const DataTypes::GeneralResponseStruct &response) override;
    /// \brief Принимает части файла осциллограммы в формате S2.
    void receiveOscFile(const S2::FileStruct &file);

    /// \brief Посылает команду записи осциллограммы с указанными из UI параметрами.
    void writeTypeOsc();
    /// \brief Запрашивает файл осциллограммы после окончания записи осциллограммы.
    void reqOscFile();

public:
    explicit OscKivDialog(Device::CurrentDevice *device, QWidget *parent = nullptr);
};

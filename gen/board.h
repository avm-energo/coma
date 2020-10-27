#pragma once

#define DEVICETYPE_MODULE 1 // модуль
#define DEVICETYPE_DEVICE 2 // прибор

#include "singleton.h"

#include <QMap>
#include <QObject>

class Board final : public QObject, public Singleton<Board>
{
    Q_OBJECT

public:
    enum DeviceType
    {
        Module,
        Device
    };

    /**
     *  Перечисление для хранения списка интерфейсов
     */
    enum InterfaceType
    {
        Unknown,
        USB,
        Ethernet,
        RS485
    };

    enum BoardType
    {
        NONE = 0,
        BASE = 1,
        MEZONIN = 2,
        BSMZ = 3,
        MODULE = 4
    };
    /**
     *  Перечисление для хранения списка приборов
     */
    enum DeviceModel
    {
        KTF = 0xA287,
        KIV = 0xA284,
        KDV = 0xA387
    };

    enum BaseBoards
    {
        MTB_00 = 0x0000,
        MTB_12 = 0x1200,
        MTB_21 = 0x2100,
        MTB_22 = 0x2200,
        MTB_31 = 0x3100,
        MTB_35 = 0x3500,
        MTB_80 = 0x8000,
        MTB_81 = 0x8100,
        MTB_84 = 0x8400,
        MTB_85 = 0x8500,
        MTB_87 = 0x8700,
        MTB_A1 = 0xA100,
        MTB_A2 = 0xA200
    };

    enum MezzanineBoards
    {
        MTM_00 = 0x0000,
        MTM_21 = 0x0021,
        MTM_22 = 0x0022,
        MTM_31 = 0x0031,
        MTM_35 = 0x0035,
        MTM_81 = 0x0081,
        MTM_82 = 0x0082,
        MTM_83 = 0x0083,
        MTM_84 = 0x0084,
        MTM_85 = 0x0085
    };

    /**
     *  Перечисление для хранения текущего состояния подключения
     */
    enum class ConnectionState
    {
        Connected,
        Closed,
        AboutToFinish
    };

    Q_ENUM(DeviceType)
    Q_ENUM(InterfaceType)
    Q_ENUM(BoardType)
    Q_ENUM(DeviceModel)
    Q_ENUM(ConnectionState)

    Q_PROPERTY(InterfaceType interface READ interfaceType WRITE setInterfaceType NOTIFY interfaceTypeChanged)
    Q_PROPERTY(DeviceType device READ deviceType WRITE setDeviceType NOTIFY deviceTypeChanged)
    Q_PROPERTY(BoardType board READ boardType WRITE setBoardType NOTIFY boardTypeChanged)
    Q_PROPERTY(ConnectionState connection READ connectionState WRITE setConnectionState NOTIFY connectionStateChanged)

    explicit Board(token);

    quint16 typeB() const;
    void setTypeB(const quint16 &typeB);

    quint16 typeM() const;
    void setTypeM(const quint16 &typeM);

    quint16 type() const;

    InterfaceType interfaceType() const;
    void setInterfaceType(InterfaceType iface);

    DeviceType deviceType() const;
    void setDeviceType(const DeviceType &deviceType);

    BoardType boardType() const;
    void setBoardType(const BoardType &boardType);

    ConnectionState connectionState() const;
    void setConnectionState(ConnectionState connectionState);

    QList<quint16> getBaseBoardsList() const;

private:
    InterfaceType m_interfaceType;
    DeviceType m_deviceType;
    BoardType m_boardType;
    ConnectionState m_connectionState;

    static const QMap<quint16, QString> m_ModuleBaseBoards()
    {
        QMap<quint16, QString> map;
        map[MTB_00] = "АВ_ТУК-00"; // нет базовой платы
        //        map[MTB_12] = {"АВ-ТУК-12 (ЦКУ)", 0x1200}; // процессорный, не комбинируется
        map[MTB_21] = "АВ-ТУК-21"; // аналоговый ввод DC, комбинируется с 2x,3x
        map[MTB_22] = "АВ-ТУК-22"; // аналоговый ввод DC, комбинируется с 2x,3x
        map[MTB_31] = "АВ-ТУК-31"; // дискретный ввод, комбинируется с 2x,3x
        map[MTB_35] = "АВ-ТУК-35"; // дискретный вывод, комбинируется с 2x,3x
        map[MTB_80] = "АВТУК-"; // аналоговый ввод AC, комбинируется с 81,82,83
        map[MTB_81] = "АВ-ТУК-81"; // МНК3, комбинируется с 84
        map[MTB_84] = "АВ-ТУК-84"; // НКВВ, комбинируется с 81,82,83
        map[MTB_85] = "АВ-ТУК-85"; // УСК, комбинируется с 85
        map[MTB_87] = "АВ-ТУК-87 (ЧР)"; // ЧР, не комбинируется
        //        map[MTB_A1] = {"ПКС-1", 0xA100}; // ПКДН, прибор для Туркота, не комбинируется
        //        map[MTB_A2] = {"АВМ", 0xA200}; // МНК3, не комбинируется
        return map;
    }

    static const QMap<quint16, QString> m_ModuleMezzanineBoards()
    {
        QMap<quint16, QString> map;
        map[MTM_00] = ""; // нет мезонина
        map[MTM_21] = "21"; // аналоговый ввод, комбинируется с 2x,3x
        map[MTM_22] = "22"; // аналоговый ввод, комбинируется с 2x,3x
        map[MTM_31] = "31"; // дискретный ввод, комбинируется с 2x,3x
        map[MTM_35] = "35"; // дискретный вывод, комбинируется с 2x,3x
        map[MTM_81] = "81 (Э2Т0Н)"; // 0U6I, комбинируется с 80,84
        map[MTM_82] = "82 (Э1Т1Н)"; // 3U3I, комбинируется с 80,84
        map[MTM_83] = "83 (Э0Т2Н)"; // 6U0I, комбинируется с 80,84
        map[MTM_84] = "84"; // 3U3I, комбинируется с 80,81
        map[MTM_85] = "85 (УСК)"; // 3U3I УСК (перегрузка по току 20х), комбинируется с 85
        return map;
    }

    quint16 m_typeB;
    quint16 m_typeM;

signals:
    void interfaceTypeChanged(Board::InterfaceType);
    void deviceTypeChanged(Board::DeviceType);
    void boardTypeChanged(Board::BoardType);
    void typeChanged();
    void connectionStateChanged(Board::ConnectionState);
};

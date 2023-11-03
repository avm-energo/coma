#ifndef HIDDENDIALOG_H
#define HIDDENDIALOG_H

#include "../widgets/udialog.h"

#include <QPaintEvent>

constexpr int RSTTIMEOUT = 5000; // таймаут на рестарт модуля после отправки ему блока Bhb

constexpr quint16 BNMN = 0x00;
constexpr quint16 BYMN = 0x01;
constexpr quint16 BNMY = 0x02;
constexpr quint16 BYMY = 0x03;

class HiddenDialog : public UDialog
{
    Q_OBJECT
private:
    struct BoardHiddenBlock
    {
        quint32 boardType;    // тип платы
        quint32 serialNum;    // серийный номер платы
        quint32 hardwareVer;  // версия аппаратного обеспечения модуля (платы)
        quint32 ModSerialNum; // серийный номер модуля целиком
    };

    struct DeviceHiddenBlock
    {
        BoardHiddenBlock baseBoardBlock;
        BoardHiddenBlock mezzBoardBlock;
    };

    QString m_BGImage;
    char m_type;
    bool m_isMezzSelected;
    bool m_isGodMode;
    DeviceHiddenBlock m_deviceBlock;

public:
    explicit HiddenDialog(QWidget *parent = nullptr);
    void fill();

private:
    void setupUI();
    void setVersion(quint32 number, QString lename);
    quint32 getVersion(QString lename);
    void sendDeviceHiddenBlock();
    void updateMode(bool enabled);

private slots:
    void acceptChanges();
    void setMezzanineEnabled(int enabled);

protected:
    void paintEvent(QPaintEvent *e);
};

#endif // HIDDENDIALOG_H

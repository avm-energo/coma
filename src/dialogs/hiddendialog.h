#ifndef HIDDENDIALOG_H
#define HIDDENDIALOG_H
#include "../widgets/udialog.h"

#include <QDialog>
#include <QPaintEvent>
#include <QVBoxLayout>

constexpr int RSTTIMEOUT = 5000; // таймаут на рестарт модуля после отправки ему блока Bhb

constexpr quint16 BNMN = 0x00;
constexpr quint16 BYMN = 0x01;
constexpr quint16 BNMY = 0x02;
constexpr quint16 BYMY = 0x03;

class HiddenDialog : public UDialog
{
    Q_OBJECT
    struct Bhb_Main
    {
        quint32 MType;        // тип платы
        quint32 SerialNum;    // серийный номер платы
        quint32 HWVer;        // версия аппаратного обеспечения модуля (платы)
        quint32 ModSerialNum; // серийный номер модуля целиком
    };
    struct Bhb_Overall
    {
        Bhb_Main BoardBBhb;
        Bhb_Main BoardMBhb;
    };
    Q_PROPERTY(bool status READ status WRITE setStatus NOTIFY statusChanged)
public:
    explicit HiddenDialog(QWidget *parent = nullptr);
    void fill();

    bool status() const
    {
        return m_status;
    }

public slots:
    void setStatus(bool status)
    {
        if (m_status == status)
            return;

        m_status = status;
        emit statusChanged(m_status);
    }

signals:
    void statusChanged(bool status);

private:
    void setupUI();
    void setVersion(quint32 number, QString lename);
    quint32 getVersion(QString lename);
    void sendBhb();
    void updateMode(bool status);

    QString m_BGImage;
    char m_type;
    bool m_withMezzanine;
    Bhb_Overall m_bhb;
    bool m_status = false;
private slots:
    void acceptChanges();
    void setMezzanineEnabled(int Enabled);

protected:
    void paintEvent(QPaintEvent *e);
};

#endif // HIDDENDIALOG_H

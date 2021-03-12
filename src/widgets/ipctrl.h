#pragma once
#include <QFrame>
#include <array>

/// Thanx to https://stackoverflow.com/a/11358560/8524139
class QLineEdit;
class IPCtrl : public QFrame
{
    Q_OBJECT
    enum
    {
        QTUTL_IP_SIZE = 4, // число октетов IP адресе
        MAX_DIGITS = 3     // число символов в LineEdit
    };

public:
    IPCtrl(QWidget *parent = 0);
    ~IPCtrl();

    virtual bool eventFilter(QObject *obj, QEvent *event);

    std::array<quint8, QTUTL_IP_SIZE> getIP() const;
    void setIP(std::array<quint8, QTUTL_IP_SIZE> ipAddr);

signals:
    void signalTextChanged(QLineEdit *pEdit);

private:
    std::array<QLineEdit *, QTUTL_IP_SIZE> m_pLineEdit;
    void slotTextChanged(QLineEdit *pEdit);
    void moveNextLineEdit(int i);
    void movePrevLineEdit(int i);
};

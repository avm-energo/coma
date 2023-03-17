#pragma once
#include <QFrame>
#include <array>

/// Thanx to https://stackoverflow.com/a/11358560/8524139
class QLineEdit;
class IPCtrl : public QFrame
{
    Q_OBJECT
    //    enum
    //    {
    //        QTUTL_IP_SIZE = 4, // число октетов IP адресе
    //        MAX_DIGITS = 3     // число символов в LineEdit
    //    };

public:
    // typedef quint8 value_type;
    // typedef std::array<quint8, QTUTL_IP_SIZE> container_type;

    static constexpr auto QTUTL_IP_SIZE = 4;
    static constexpr auto MAX_DIGITS = 3;
    using ip_container = std::array<quint8, QTUTL_IP_SIZE>;

    IPCtrl(QWidget *parent = 0);
    ~IPCtrl();

    virtual bool eventFilter(QObject *obj, QEvent *event);

    ip_container getIP() const;
    void setIP(const ip_container ipAddr);

signals:
    void signalTextChanged(QLineEdit *pEdit);

private:
    std::array<QLineEdit *, QTUTL_IP_SIZE> m_pLineEdit;
    void slotTextChanged(QLineEdit *pEdit);
    void moveNextLineEdit(int i);
    void movePrevLineEdit(int i);
};

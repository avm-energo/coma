#ifndef UDIALOG_H
#define UDIALOG_H

// simple dialog class to populate from

#include "uwidget.h"

class UDialog : public UWidget
{
    Q_OBJECT
public:
    UDialog(QWidget *parent = nullptr);
    UDialog(const QString &hash, const QString &key, QWidget *parent = nullptr);
    void updateConnection(Connection *conn);
    virtual void updateGeneralResponse(const DataTypes::GeneralResponseStruct &response);

    void disableSuccessMessage();
    void enableSuccessMessage();
    bool disableMessages();
    bool enableMessages();

protected:
    QString successMsg() const;
    void setSuccessMsg(const QString successMsg);

    QString errorMsg() const;
    void setErrorMsg(const QString errorMsg);

private:
    QString m_successMsg;
    QString m_errorMsg;
    bool showSuccessMessageFlag;
    QMetaObject::Connection m_genRespConn;
};

#endif // UDIALOG_H

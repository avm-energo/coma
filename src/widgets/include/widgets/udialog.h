#pragma once

#include <widgets/uwidget.h>

// simple dialog class to populate from
class UDialog : public UWidget
{
    Q_OBJECT
public:
    explicit UDialog(Device::CurrentDevice *device, QWidget *parent = nullptr);
    virtual void updateGeneralResponse(const DataTypes::GeneralResponseStruct &response);

    void disableSuccessMessage();
    void enableSuccessMessage();
    bool disableMessages();
    bool enableMessages();

    QString getFilenameForDevice() const;

protected:
    QString successMsg() const;
    void setSuccessMsg(const QString &successMsg);
    QString errorMsg() const;
    void setErrorMsg(const QString &errorMsg);

private:
    QString m_successMsg;
    QString m_errorMsg;
    bool showSuccessMessageFlag;
    QMetaObject::Connection m_genRespConn;
};

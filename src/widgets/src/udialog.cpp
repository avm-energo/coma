#include "widgets/udialog.h"

#include <device/current_device.h>
#include <gen/error.h>
#include <widgets/epopup.h>

UDialog::UDialog(Device::CurrentDevice *device, QWidget *parent) : UWidget(device, parent)
{
    showSuccessMessageFlag = true;
    setSuccessMsg("Записано успешно");
    setErrorMsg("При записи произошла ошибка");
    m_genRespConn = m_device->async()->connection(this, &UDialog::updateGeneralResponse);
}

void UDialog::updateGeneralResponse(const DataTypes::GeneralResponseStruct &response)
{
    if (!updatesEnabled())
        return;

    switch (response.type)
    {
    case DataTypes::Ok:
    {
        if (successMsg().isEmpty())
            break;
        EMessageBox::information(this, successMsg());
        break;
    }
    case DataTypes::Error:
    {
        QString msg {};
        auto errorCode = Error::Msg(response.data);
        if (errorCode == Error::Msg::FlashError && !(m_device->bsi().Cfcrc))
            msg = tr("Запрошенный файл отсутствует");
        else
            msg = Error::MsgStr.value(errorCode, "Неизвестная ошибка");
        EMessageBox::error(this, errorMsg() + " : " + msg);
        break;
    }
    default:
        break;
    }
}

void UDialog::disableSuccessMessage()
{
    showSuccessMessageFlag = false;
}

void UDialog::enableSuccessMessage()
{
    showSuccessMessageFlag = true;
}

bool UDialog::disableMessages()
{
    return QObject::disconnect(m_genRespConn);
}

bool UDialog::enableMessages()
{
    m_genRespConn = m_dataUpdater->currentConnection()->connection(this, &UDialog::updateGeneralResponse);
    return m_genRespConn;
}

QString UDialog::successMsg() const
{
    return showSuccessMessageFlag ? m_successMsg : ""; // while empty string message will not appear
}

void UDialog::setSuccessMsg(const QString &successMsg)
{
    m_successMsg = successMsg;
}

QString UDialog::errorMsg() const
{
    return m_errorMsg;
}

void UDialog::setErrorMsg(const QString &errorMsg)
{
    m_errorMsg = errorMsg;
}

QString UDialog::getFilenameForDevice() const
{
    Q_ASSERT(m_device != nullptr);
    const auto &bsi = m_device->bsi();
    return QString("%1%2-%3")
        .arg(bsi.MTypeB, 2, 16, QChar('0'))
        .arg(bsi.MTypeM, 2, 16, QChar('0'))
        .arg(bsi.SerialNum, 8, 10, QChar('0'));
}

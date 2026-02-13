#include <avm-widgets/emessagebox.h>
#include <comawidgets/udialog.h>
#include <device/current_device.h>
#include <avm-gen/error.h>

UDialog::UDialog(Device::CurrentDevice *device, QWidget *parent) : UWidget(device, parent)
{
    m_showSuccessMessageFlag = true;
    m_showOnceMessage = false;
    setSuccessMsg("Записано успешно");
    setErrorMsg("При записи произошла ошибка");
    enableMessages();
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
        qCritical() << errorMsg() << " : " << msg;
        break;
    }
    default:
        break;
    }
    m_showOnceMessage = false;
}

void UDialog::enableOnceMessage()
{
    m_showOnceMessage = true;
}

void UDialog::disableMessages()
{
    QObject::disconnect(m_genRespConn);
    disableUpdating();
}

void UDialog::enableMessages()
{
    if (!m_genRespConn)
        m_genRespConn = m_dataUpdater->currentConnection()->connection(this, &UDialog::updateGeneralResponse);
    enableUpdating();
}

QString UDialog::successMsg() const
{
    return (m_showSuccessMessageFlag || m_showOnceMessage) ? m_successMsg
                                                           : ""; // while empty string message will not appear
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
        .arg(bsi.SerialNum, 8, 16, QChar('0'));
}

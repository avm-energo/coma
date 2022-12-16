#include "udialog.h"

#include "../module/board.h"
#include "epopup.h"

#include <QMessageBox>
#include <QSettings>
#include <gen/datamanager/typesproxy.h>

UDialog::UDialog(QWidget *parent) : UWidget(parent), proxyGRS(new DataTypesProxy())
{
    proxyGRS->RegisterType<DataTypes::GeneralResponseStruct>();
    setSuccessMsg("Записано успешно");
    setErrorMsg("При записи произошла ошибка");
    connect(proxyGRS.get(), &DataTypesProxy::DataStorable, this, &UDialog::updateGeneralResponse);
}

UDialog::UDialog(const QString hash, const QString key, QWidget *parent) : UDialog(parent)
{
    auto sets = std::make_unique<QSettings>();
    auto value = sets->value(key, "").toString();
    if (value.isEmpty())
        sets->setValue(key, hash);
    m_hash = sets->value(key, "").toString();
}

// void UDialog::updateGeneralResponse(const DataTypes::GeneralResponseStruct &response)
void UDialog::updateGeneralResponse(const QVariant &msg)
{
    if (!updatesEnabled())
        return;

    auto response = msg.value<DataTypes::GeneralResponseStruct>();
    switch (response.type)
    {
    case DataTypes::Ok:
    {
        if (successMsg().isEmpty())
            break;
        QMessageBox::information(this, "Information", successMsg());
        // EMessageBox::information(this, successMsg());
        break;
    }
    case DataTypes::Error:
    {
        auto code = Error::Msg(response.data);
        QString msg {};
        switch (code)
        {
        case Error::FlashError:
        {
            if (!Board::GetInstance().isCrcValid())
            {
                msg = tr("Запрошенный файл отсутствует");
                break;
            }
            [[fallthrough]];
        }
        default:
            msg = Error::MsgStr[Error::Msg(response.data)];
            break;
        }
        QMessageBox::critical(this, "Error", errorMsg() + " : " + msg);
        // EMessageBox::warning(this, errorMsg() + " : " + msg);
        break;
    }
    default:
    {
        break;
    }
    }
}

QString UDialog::successMsg() const
{
    return m_successMsg;
}

void UDialog::setSuccessMsg(const QString successMsg)
{
    m_successMsg = successMsg;
}

QString UDialog::errorMsg() const
{
    return m_errorMsg;
}

void UDialog::setErrorMsg(const QString errorMsg)
{
    m_errorMsg = errorMsg;
}

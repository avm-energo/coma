#include "udialog.h"

#include "../gen/board.h"
#include "../s2/datamanager.h"
#include "epopup.h"

//#include <QMessageBox>
#include <QSettings>
UDialog::UDialog(QWidget *parent) : UWidget(parent)
{
    const auto &manager = DataManager::GetInstance();
    setSuccessMsg("Записано успешно");
    setErrorMsg("При записи произошла ошибка");
    connect(&manager, &DataManager::responseReceived, this, &UDialog::updateGeneralResponse);
}

UDialog::UDialog(const QString hash, const QString key, QWidget *parent) : UDialog(parent)
{
    auto sets = std::make_unique<QSettings>();
    auto value = sets->value(key, "").toString();
    if (value.isEmpty())
        sets->setValue(key, hash);
    m_hash = sets->value(key, "").toString();
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
        EMessageBox::warning(this, errorMsg() + " : " + msg);
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

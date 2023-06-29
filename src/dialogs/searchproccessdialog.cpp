#include "searchproccessdialog.h"

#include "../widgets/wd_func.h"

#include <QStandardItemModel>
#include <QTimer>

SearchProccessDialog::SearchProccessDialog(const SearchParams &data, QWidget *parent)
    : QDialog(parent), params(data), timeoutTimer(new QTimer(this))
{
    setObjectName("rsSearchProccessDialog");
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Поиск устройств");
    setupUI();
}

void SearchProccessDialog::setupUI()
{
    auto mainLayout = new QVBoxLayout;
    auto tableViewModel = new QStandardItemModel(this);
    auto tableView = WDFunc::NewQTV(this, "devicesTable", tableViewModel);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(tableView);
    setLayout(mainLayout);
    adjustSize();
}

void SearchProccessDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    search();
}

void SearchProccessDialog::errorHandler(QSerialPort::SerialPortError error)
{
    switch (error)
    {
    case QSerialPort::TimeoutError:
        break;
    case QSerialPort::NotOpenError:
        break;
    case QSerialPort::ResourceError:
        break;
    default:
        qDebug() << QVariant::fromValue(error).toString();
        break;
    }
}

void SearchProccessDialog::receiveResponse(QSerialPort *port)
{
    ;
}

void SearchProccessDialog::search()
{
    constexpr auto bufferSize = 1024;
    for (const auto &baud : params.bauds)
    {
        for (const auto &parity : params.parities)
        {
            for (const auto &stopBit : params.stopBits)
            {
                auto port = new QSerialPort(params.port, this);
                port->setFlowControl(QSerialPort::NoFlowControl);
                port->setDataBits(QSerialPort::Data8);
                port->setReadBufferSize(bufferSize);
                port->setBaudRate(baud);
                port->setParity(parity);
                port->setStopBits(stopBit);
                QObject::connect(port, &QSerialPort::errorOccurred, this, &SearchProccessDialog::errorHandler);
                QObject::connect(port, &QIODevice::readyRead, this, [this, port] { receiveResponse(port); });
                for (auto addr = params.startAddr; addr <= params.endAddr; addr++)
                {
                    ;
                }
            }
        }
    }
}

#include "searchproccessdialog.h"

#include "../widgets/wd_func.h"

#include <QCoreApplication>
#include <QStandardItemModel>
#include <QTimer>

SearchProccessDialog::SearchProccessDialog(const SearchParams &data, QWidget *parent)
    : QDialog(parent)
    , params(data)
    , timeoutTimer(new QTimer(this))
    , timeout(false)
    , responseReceived(false)
    , portError(false)
{
    timeoutTimer->setSingleShot(true);
    timeoutTimer->setInterval(params.timeout);
    QObject::connect(timeoutTimer, &QTimer::timeout, this, [this] { timeout = true; });

    setObjectName("rsSearchProccessDialog");
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Поиск устройств");
    setupUI();
}

void SearchProccessDialog::setupUI()
{
    auto mainLayout = new QVBoxLayout;
    auto tableViewModel = new QStandardItemModel(this);
    QStringList headers { "Порт", "Адрес", "Скорость", "Чётность", "Стоп бит", "Статус" };
    tableViewModel->setHorizontalHeaderLabels(headers);
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
    timeoutTimer->stop();
    switch (error)
    {
    case QSerialPort::TimeoutError:
        timeout = true;
        break;
    case QSerialPort::OpenError:
    case QSerialPort::NotOpenError:
    case QSerialPort::ResourceError:
        portError = true;
        break;
    default:
        qWarning() << QVariant::fromValue(error).toString();
        break;
    }
}

void SearchProccessDialog::receiveResponse(QSerialPort *port)
{
    Q_UNUSED(port);
    //
    responseReceived = true;
}

QByteArray SearchProccessDialog::createRequest(int address)
{
    constexpr char body[] = { 0x04, 0x00, 0x01, 0x00, 0x1e, 0x21, static_cast<char>(0xc2) };
    const auto oneByteAddr = static_cast<char>(address);
    auto request = QByteArray::fromRawData(&body[0], sizeof(body));
    request = request.prepend(oneByteAddr);
    return request;
}

void SearchProccessDialog::createModelItem(quint32 row, int addr, int baud, //
    QSerialPort::Parity parity, QSerialPort::StopBits stopBit)
{
    auto tableView = findChild<QTableView *>("devicesTable");
    if (tableView)
    {
        auto tableViewModel = static_cast<QStandardItemModel *>(tableView->model());
        // tableViewModel->appendRow()
        if (tableViewModel)
        {
            tableViewModel->setData(tableViewModel->index(row, 0), params.port);
            tableViewModel->setData(tableViewModel->index(row, 1), QString::number(addr));
            tableViewModel->setData(tableViewModel->index(row, 2), QString::number(baud));
            tableViewModel->setData(tableViewModel->index(row, 5), "Wait...");

            auto parityIndex = tableViewModel->index(row, 3);
            if (parity == QSerialPort::Parity::NoParity)
                tableViewModel->setData(parityIndex, "Нет");
            else if (parity == QSerialPort::Parity::OddParity)
                tableViewModel->setData(parityIndex, "Нечёт");
            else
                tableViewModel->setData(parityIndex, "Чёт");

            auto stopBitIndex = tableViewModel->index(row, 4);
            if (stopBit == QSerialPort::StopBits::OneStop)
                tableViewModel->setData(stopBitIndex, "1");
            else
                tableViewModel->setData(stopBitIndex, "2");
        }
        tableView->setModel(tableViewModel);
        // tableView->repaint();
        QCoreApplication::processEvents();
    }
}

void SearchProccessDialog::search()
{
    constexpr auto bufferSize = 1024;
    quint32 count = 0;
    for (const auto baud : params.bauds)
    {
        for (const auto parity : params.parities)
        {
            for (const auto stopBit : params.stopBits)
            {
                auto port = new QSerialPort(params.port, this);
                QObject::connect(port, &QSerialPort::errorOccurred, this, &SearchProccessDialog::errorHandler);
                QObject::connect(port, &QIODevice::readyRead, this, [this, port] { receiveResponse(port); });
                port->setFlowControl(QSerialPort::NoFlowControl);
                port->setDataBits(QSerialPort::Data8);
                port->setReadBufferSize(bufferSize);
                port->setBaudRate(baud);
                port->setParity(parity);
                port->setStopBits(stopBit);
                for (auto addr = params.startAddr; addr <= params.endAddr; addr++)
                {
                    // TODO: отобразить соединение в таблице
                    createModelItem(count, addr, baud, parity, stopBit);
                    auto openStatus = port->open(QIODevice::ReadWrite);
                    if (openStatus)
                    {
                        auto request = createRequest(addr);
                        port->write(request);
                        timeoutTimer->start();
                        while (!timeout && !responseReceived)
                        {
                            QCoreApplication::processEvents();
                        }
                    }
                    else
                        portError = true;

                    if (portError)
                    {
                        ;
                    }
                    if (timeout)
                    {
                        ;
                    }
                    if (responseReceived)
                    {
                        ;
                    }
                    count++;
                }
                port->deleteLater();
            }
        }
    }
}

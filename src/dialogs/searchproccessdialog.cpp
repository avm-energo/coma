#include "searchproccessdialog.h"

#include "../widgets/epopup.h"
#include "../widgets/wd_func.h"

#include <QCoreApplication>
#include <QProgressBar>
#include <QStandardItemModel>
#include <QTimer>

SearchProccessDialog::SearchProccessDialog(const SearchParams &data, QWidget *parent)
    : QDialog(parent)
    , params(data)
    , timeoutTimer(new QTimer(this))
    , tableView(nullptr)
    , progressBar(nullptr)
    , timeout(false)
    , responseReceived(false)
    , portError(false)
{
    timeoutTimer->setSingleShot(true);
    timeoutTimer->setInterval(params.timeout);
    QObject::connect(timeoutTimer, &QTimer::timeout, this, [this] { timeout = true; });

    setObjectName("rsSearchProccessDialog");
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_ShowModal);
    setWindowTitle("Поиск устройств");
    setupUI();
}

void SearchProccessDialog::setupUI()
{
    auto mainLayout = new QVBoxLayout;
    auto tableViewModel = new QStandardItemModel(this);
    QStringList headers { "Порт", "Адрес", "Скорость", "Чётность", "Стоп бит", "Статус" };
    tableViewModel->setHorizontalHeaderLabels(headers);
    tableView = WDFunc::NewQTV(this, "devicesTable", tableViewModel);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(tableView);
    progressBar = new QProgressBar(this);
    progressBar->setOrientation(Qt::Horizontal);
    mainLayout->addWidget(progressBar);
    setLayout(mainLayout);
    adjustSize();
}

void SearchProccessDialog::errorHandler(QSerialPort::SerialPortError error)
{
    timeoutTimer->stop();
    switch (error)
    {
    case QSerialPort::SerialPortError::NoError:
        // ignore no error
        break;
    case QSerialPort::SerialPortError::TimeoutError:
        timeout = true;
        break;
    case QSerialPort::SerialPortError::OpenError:
    case QSerialPort::SerialPortError::NotOpenError:
    case QSerialPort::SerialPortError::ResourceError:
        portError = true;
        break;
    default:
        qWarning() << QVariant::fromValue(error).toString();
        break;
    }
}

void SearchProccessDialog::receiveResponse(QSerialPort *port)
{
    if (port->isOpen())
    {
        // TODO: Можно анализировать ответ от устройства
        [[maybe_unused]] auto received = port->readAll();
        responseReceived = true;
    }
    else
        portError = true;
}

QByteArray SearchProccessDialog::createRequest(int address)
{
    constexpr char body[] = { 0x04, 0x00, 0x01, 0x00, 0x1e, 0x21, static_cast<char>(0xc2) };
    const auto oneByteAddr = static_cast<char>(address);
    auto request = QByteArray::fromRawData(&body[0], sizeof(body));
    request = request.prepend(oneByteAddr);
    return request;
}

void SearchProccessDialog::createModelItem(int addr, int baud, //
    QSerialPort::Parity parity, QSerialPort::StopBits stopBit)
{
    auto model = static_cast<QStandardItemModel *>(tableView->model());
    if (model)
    {
        QList<QStandardItem *> items {
            new QStandardItem(params.port),           //
            new QStandardItem(QString::number(addr)), //
            new QStandardItem(QString::number(baud)), //
        };

        if (parity == QSerialPort::Parity::NoParity)
            items.append(new QStandardItem("Нет"));
        else if (parity == QSerialPort::Parity::OddParity)
            items.append(new QStandardItem("Нечёт"));
        else
            items.append(new QStandardItem("Чёт"));

        if (stopBit == QSerialPort::StopBits::OneStop)
            items.append(new QStandardItem("1"));
        else
            items.append(new QStandardItem("2"));
        items.append(new QStandardItem("Wait..."));
        model->appendRow(items);
    }
    tableView->setModel(model);
    QCoreApplication::processEvents();
}

void SearchProccessDialog::sendRequest(QSerialPort *port, int addr)
{
    if (port->isOpen())
    {
        auto request = createRequest(addr);
        port->write(request);
        timeoutTimer->start();
        while (!timeout && !responseReceived)
            QCoreApplication::processEvents();
    }
    else
        portError = true;
    QCoreApplication::processEvents();
}

void SearchProccessDialog::updateTable(quint32 row)
{
    auto model = static_cast<QStandardItemModel *>(tableView->model());
    QColor red(0xf96f6f);
    QColor green(0x00cc66);

    auto statusIndex = model->index(row, 5);
    if (portError)
        model->setData(statusIndex, "Error", Qt::DisplayRole);
    else if (timeout)
        model->setData(statusIndex, "Timeout", Qt::DisplayRole);
    else if (responseReceived)
        model->setData(statusIndex, "Ok", Qt::DisplayRole);

    for (int col = 0; col < model->columnCount(); col++)
    {
        auto itemIndex = model->index(row, col);
        if (portError || timeout)
            model->setData(itemIndex, red, Qt::BackgroundRole);
        else if (responseReceived)
            model->setData(itemIndex, green, Qt::BackgroundRole);
    }
    updateProgressBar();

    portError = false;
    timeout = false;
    responseReceived = false;
    QCoreApplication::processEvents();
}

void SearchProccessDialog::setMaxProgressBar()
{
    auto size = params.bauds.size() * params.parities.size() //
        * params.stopBits.size()                             //
        * (params.endAddr - params.startAddr + 1);           //
    progressBar->setMinimum(0);
    progressBar->setValue(0);
    progressBar->setMaximum(size);
}

void SearchProccessDialog::updateProgressBar()
{
    progressBar->setValue(progressBar->value() + 1);
}

void SearchProccessDialog::search()
{
    constexpr auto bufferSize = 1024;
    quint32 row = 0;
    setMaxProgressBar();
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
                auto openStatus = port->open(QIODevice::ReadWrite);
                for (auto addr = params.startAddr; addr <= params.endAddr; addr++)
                {
                    createModelItem(addr, baud, parity, stopBit);
                    if (openStatus)
                        sendRequest(port, addr);
                    else
                        portError = true;
                    updateTable(row);
                    row++;
                }
                port->flush();
                port->close();
                port->deleteLater();
                QCoreApplication::processEvents();
            }
        }
    }
    EMessageBox::information(this, "Сканирование завершено!");
}

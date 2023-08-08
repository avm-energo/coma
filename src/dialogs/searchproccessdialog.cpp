#include "searchproccessdialog.h"

#include "../widgets/epopup.h"
#include "../widgets/wd_func.h"

#include <QCoreApplication>
#include <QHeaderView>
#include <QProgressBar>
#include <QPushButton>
#include <QStandardItemModel>
#include <QThread>
#include <QTimer>
#include <gen/stdfunc.h>
#include <gen/utils/crc16.h>

SearchProccessDialog::SearchProccessDialog(const SearchParams &data, QWidget *parent)
    : QDialog(parent)
    , params(data)
    , timeoutTimer(new QTimer(this))
    , tableView(nullptr)
    , progressBar(nullptr)
    , expectedResponseSize(0)
    , currentAddress(0)
    , timeout(false)
    , responseReceived(false)
    , responseError(false)
    , portError(false)
    , stop(false)
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
    auto stopButton = new QPushButton("Остановить поиск", this);
    QObject::connect(stopButton, &QPushButton::clicked, this, [this]() {
        if (!stop)
        {
            stop = true;
            EMessageBox::information(this, "Поиск остановлен!");
        }
    });
    mainLayout->addWidget(stopButton);
    setLayout(mainLayout);
    setMinimumSize(700, 600);
}

void SearchProccessDialog::errorHandler(const QSerialPort::SerialPortError error)
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
    default:
        portError = true;
        qWarning() << QVariant::fromValue(error).toString();
        break;
    }
}

void SearchProccessDialog::receiveResponse(QSerialPort *port)
{
    if (port->isOpen())
    {
        while (port->bytesAvailable() && !timeout && !stop)
        {
            response.append(port->readAll());
            QCoreApplication::processEvents();
        }
        if (response.size() >= expectedResponseSize)
        {
            timeoutTimer->stop();
            // Анализируем ответ от устройства
            if (analyzeResponse(response))
                responseReceived = true;
            else
                responseError = true;
        }
    }
    else
        portError = true;
}

bool SearchProccessDialog::analyzeResponse(const QByteArray &actualResponse)
{
    constexpr auto expectedSize = (0x1e * 2) + 5;
    if (actualResponse.size() == expectedSize)
    {
        const auto addr = static_cast<quint8>(actualResponse[0]);
        const auto funcCode = static_cast<quint8>(actualResponse[1]);
        const auto size = static_cast<quint8>(actualResponse[2]);
        if (addr == currentAddress && funcCode == 0x04 && size == (0x1e * 2))
        {
            auto crcBytes = actualResponse.right(sizeof(quint16));
            auto dataBytes = actualResponse.left(actualResponse.size() - sizeof(quint16));
            quint16 actualCrc = ((crcBytes[0] << 8) | crcBytes[1]);
            utils::CRC16 expectedCrc(dataBytes);
            if (expectedCrc == actualCrc)
                return true;
        }
    }
    return false;
}

QByteArray SearchProccessDialog::createRequest()
{
    constexpr char body[] = { 0x04, 0x00, 0x01, 0x00, 0x1e };
    auto request = QByteArray::fromRawData(&body[0], sizeof(body));
    request = request.prepend(currentAddress);
    utils::CRC16 crc(request);
    request.append(crc.toByteArray());
    expectedResponseSize = (0x1e * 2) + 5;
    return request;
}

void SearchProccessDialog::createModelItem(quint32 row, int addr, int baud, //
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
        tableView->scrollTo(model->index(row, 0));
    }
    QCoreApplication::processEvents();
}

void SearchProccessDialog::sendRequest(QSerialPort *port)
{
    response.clear();
    if (port->isOpen())
    {
        auto request = createRequest();
        port->write(request);
        timeoutTimer->start();
        while (!timeout && !responseReceived && !responseError && !portError && !stop)
            QCoreApplication::processEvents();
        if (portError)
            timeout = false;
    }
    else
        portError = true;
    QCoreApplication::processEvents();
}

void SearchProccessDialog::updateTable(quint32 row)
{
    auto model = static_cast<QStandardItemModel *>(tableView->model());
    QColor red(0xf96f6f);
    QColor yellow(0xfffd99);
    QColor green(0x029939);

    auto statusIndex = model->index(row, 5);
    if (portError)
    {
        model->setData(statusIndex, "Error", Qt::DisplayRole);
        model->setData(statusIndex, QIcon(":/icons/tnno.svg"), Qt::DecorationRole);
    }
    else if (timeout)
    {
        model->setData(statusIndex, "Timeout", Qt::DisplayRole);
        model->setData(statusIndex, QIcon(":/icons/tnno.svg"), Qt::DecorationRole);
    }
    else if (responseError)
    {
        model->setData(statusIndex, "Response error", Qt::DisplayRole);
        model->setData(statusIndex, QIcon(":/icons/tnno.svg"), Qt::DecorationRole);
    }
    else if (responseReceived)
    {
        model->setData(statusIndex, "Ok", Qt::DisplayRole);
        model->setData(statusIndex, QIcon(":/icons/tnyes.svg"), Qt::DecorationRole);
    }

    for (int col = 0; col < model->columnCount(); col++)
    {
        auto itemIndex = model->index(row, col);
        if (portError || timeout)
            model->setData(itemIndex, red, Qt::BackgroundRole);
        else if (responseError)
            model->setData(itemIndex, yellow, Qt::BackgroundRole);
        else if (responseReceived)
            model->setData(itemIndex, green, Qt::BackgroundRole);
    }

    timeout = false;
    responseError = false;
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

void SearchProccessDialog::searchFinish(QSerialPort *port)
{
    port->flush();
    port->close();
    port->deleteLater();
    progressBar->setValue(progressBar->maximum());
}

void SearchProccessDialog::search()
{
    constexpr auto bufferSize = 1024;
    auto port = new QSerialPort(params.port, this);
    QObject::connect(port, &QSerialPort::errorOccurred, this, &SearchProccessDialog::errorHandler);
    QObject::connect(port, &QIODevice::readyRead, this, [this, port] { receiveResponse(port); });
    port->setFlowControl(QSerialPort::FlowControl::NoFlowControl);
    port->setDataBits(QSerialPort::Data8);
    port->setReadBufferSize(bufferSize);

    quint32 row = 0;
    setMaxProgressBar();
    for (const auto baud : params.bauds)
    {
        for (const auto parity : params.parities)
        {
            for (const auto stopBit : params.stopBits)
            {
                port->setBaudRate(baud);
                port->setParity(parity);
                port->setStopBits(stopBit);
                auto openStatus = port->open(QIODevice::ReadWrite);
                for (auto addr = params.startAddr; addr <= params.endAddr; addr++)
                {
                    // При закрытии окна или остановке поиска через кнопку
                    if (stop)
                    {
                        searchFinish(port);
                        return;
                    }

                    currentAddress = static_cast<quint8>(addr);
                    createModelItem(row, addr, baud, parity, stopBit);
                    if (openStatus)
                        sendRequest(port);
                    else
                        portError = true;
                    updateTable(row);
                    updateProgressBar();
                    row++;

                    // При неожиданной ошибке порта закрываем соединение
                    if (portError)
                    {
                        searchFinish(port);
                        EMessageBox::error(this, "Произошла ошибка COM-порта!");
                        return;
                    }
                }
                port->flush();
                port->close();
                QCoreApplication::processEvents();
            }
        }
    }
    // Поиск закончился
    stop = true;
    port->deleteLater();
    EMessageBox::information(this, "Сканирование завершено!");
}

void SearchProccessDialog::done(int r)
{
    // Если поиск не закончился
    if (!stop)
    {
        stop = true;
        auto closeTimer = new QTimer(this);
        closeTimer->setSingleShot(true);
        QObject::connect(closeTimer, &QTimer::timeout, this, [this, r](const auto) { QDialog::done(r); });
        closeTimer->start(params.timeout);
    }
    else
        QDialog::done(r);
}

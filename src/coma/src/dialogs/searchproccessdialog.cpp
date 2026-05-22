#include "dialogs/searchproccessdialog.h"

#include "const.h"
#include "dialogs/interfaceserialdialog.h"
#include <avm-gen/stdfunc.h>
#include <avm-gen/utils/crc16.h>
#include <avm-widgets/emessagebox.h>
#include <avm-widgets/tvfunc.h>

#include <QCoreApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QMenu>
#include <QProgressBar>
#include <QPushButton>
#include <QStandardItemModel>
#include <QThread>
#include <QTimer>

SearchProccessDialog::SearchProccessDialog(
    const SearchParams &data, InterfaceSerialDialog *targetDialog, QWidget *parent)
    : QDialog(parent)
    , m_params(data)
    , m_timeoutTimer(new QTimer(this))
    , m_tableView(nullptr)
    , m_progressBar(nullptr)
    , m_expectedResponseSize(0)
    , m_currentAddress(0)
    , m_timeout(false)
    , m_responseReceived(false)
    , m_responseError(false)
    , m_portError(false)
    , m_stop(false)
    , m_targetDialog(targetDialog)
{
    m_timeoutTimer->setSingleShot(true);
    m_timeoutTimer->setInterval(m_params.timeout);
    QObject::connect(m_timeoutTimer, &QTimer::timeout, this, [this] { m_timeout = true; });
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
    m_tableView = TVFunc::New(this, "devicesTable", tableViewModel);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(
        m_tableView, &QTableView::customContextMenuRequested, this, &SearchProccessDialog::showContextMenu);

    mainLayout->addWidget(m_tableView);
    m_progressBar = new QProgressBar(this);
    m_progressBar->setOrientation(Qt::Horizontal);
    mainLayout->addWidget(m_progressBar);
    auto stopButton = new QPushButton("Остановить поиск", this);
    QObject::connect(stopButton, &QPushButton::clicked, this,
        [this]()
        {
            if (!m_stop)
            {
                m_stop = true;
                EMessageBox::information(this, "Поиск остановлен!");
            }
        });
    mainLayout->addWidget(stopButton);
    setLayout(mainLayout);
    setMinimumSize(700, 600);
}

void SearchProccessDialog::errorHandler(const QSerialPort::SerialPortError error)
{
    m_timeoutTimer->stop();
    switch (error)
    {
    case QSerialPort::SerialPortError::NoError:
        // ignore no error
        break;
    case QSerialPort::SerialPortError::TimeoutError:
        m_timeout = true;
        break;
    default:
        m_portError = true;
        qWarning() << QVariant::fromValue(error).toString();
        break;
    }
}

void SearchProccessDialog::receiveResponse(QSerialPort *port)
{
    if (port->isOpen())
    {
        while (port->bytesAvailable() && !m_timeout && !m_stop)
        {
            m_response.append(port->readAll());
            QCoreApplication::processEvents();
        }
        if (m_response.size() >= m_expectedResponseSize)
        {
            m_timeoutTimer->stop();
            // Анализируем ответ от устройства
            if (analyzeResponse(m_response))
                m_responseReceived = true;
            else
                m_responseError = true;
        }
    }
    else
        m_portError = true;
}

bool SearchProccessDialog::analyzeResponse(const QByteArray &actualResponse)
{
    constexpr auto expectedSize = (0x1e * 2) + 5;
    if (actualResponse.size() == expectedSize)
    {
        const auto addr = static_cast<quint8>(actualResponse[0]);
        const auto funcCode = static_cast<quint8>(actualResponse[1]);
        const auto size = static_cast<quint8>(actualResponse[2]);
        if (addr == m_currentAddress && funcCode == 0x04 && size == (0x1e * 2))
        {
            auto crcBytes = actualResponse.right(sizeof(quint16));
            auto dataBytes = actualResponse.left(actualResponse.size() - sizeof(quint16));
            quint16 actualCrc = ((static_cast<quint8>(crcBytes[0]) << 8) | static_cast<quint8>(crcBytes[1]));
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
    request = request.prepend(m_currentAddress);
    utils::CRC16 crc(request);
    request.append(crc.toByteArray());
    m_expectedResponseSize = (0x1e * 2) + 5;
    return request;
}

void SearchProccessDialog::createModelItem(quint32 row, int addr, int baud, //
    QSerialPort::Parity parity, QSerialPort::StopBits stopBit)
{
    auto model = static_cast<QStandardItemModel *>(m_tableView->model());
    if (model)
    {
        QList<QStandardItem *> items {
            new QStandardItem(m_params.port),         //
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
        m_tableView->scrollTo(model->index(row, 0));
    }
    QCoreApplication::processEvents();
}

void SearchProccessDialog::sendRequest(QSerialPort *port)
{
    m_response.clear();
    if (port->isOpen())
    {
        auto request = createRequest();
        port->write(request);
        m_timeoutTimer->start();
        while (!m_timeout && !m_responseReceived && !m_responseError && !m_portError && !m_stop)
            QCoreApplication::processEvents();
        if (m_portError)
            m_timeout = false;
    }
    else
        m_portError = true;
    QCoreApplication::processEvents();
}

void SearchProccessDialog::updateTable(quint32 row)
{
    auto model = static_cast<QStandardItemModel *>(m_tableView->model());
    QColor red(0xf96f6f);
    QColor yellow(0xfffd99);
    QColor green(0x029939);

    auto statusIndex = model->index(row, 5);
    if (m_portError)
    {
        model->setData(statusIndex, StatusIndexRS485::Error, Qt::DisplayRole);
        model->setData(statusIndex, QIcon(":/icons/tnno.svg"), Qt::DecorationRole);
    }
    else if (m_timeout)
    {
        model->setData(statusIndex, StatusIndexRS485::Timeout, Qt::DisplayRole);
        model->setData(statusIndex, QIcon(":/icons/tnno.svg"), Qt::DecorationRole);
    }
    else if (m_responseError)
    {
        model->setData(statusIndex, StatusIndexRS485::ResponseError, Qt::DisplayRole);
        model->setData(statusIndex, QIcon(":/icons/tnno.svg"), Qt::DecorationRole);
    }
    else if (m_responseReceived)
    {
        model->setData(statusIndex, StatusIndexRS485::Ok, Qt::DisplayRole);
        model->setData(statusIndex, QIcon(":/icons/tnyes.svg"), Qt::DecorationRole);
    }

    for (int col = 0; col < model->columnCount(); col++)
    {
        auto itemIndex = model->index(row, col);
        if (m_portError || m_timeout)
            model->setData(itemIndex, red, Qt::BackgroundRole);
        else if (m_responseError)
            model->setData(itemIndex, yellow, Qt::BackgroundRole);
        else if (m_responseReceived)
            model->setData(itemIndex, green, Qt::BackgroundRole);
    }

    m_timeout = false;
    m_responseError = false;
    m_responseReceived = false;
    QCoreApplication::processEvents();
}

void SearchProccessDialog::setMaxProgressBar()
{
    auto size = m_params.bauds.size() * m_params.parities.size() //
        * m_params.stopBits.size()                               //
        * (m_params.endAddr - m_params.startAddr + 1);           //
    m_progressBar->setMinimum(0);
    m_progressBar->setValue(0);
    m_progressBar->setMaximum(size);
}

void SearchProccessDialog::updateProgressBar()
{
    m_progressBar->setValue(m_progressBar->value() + 1);
}

void SearchProccessDialog::searchFinish(QSerialPort *port)
{
    port->flush();
    port->close();
    port->deleteLater();
    m_progressBar->setValue(m_progressBar->maximum());
}

void SearchProccessDialog::search()
{
    constexpr auto bufferSize = 1024;
    auto port = new QSerialPort(m_params.port, this);
    QObject::connect(port, &QSerialPort::errorOccurred, this, &SearchProccessDialog::errorHandler);
    QObject::connect(port, &QIODevice::readyRead, this, [this, port] { receiveResponse(port); });
    port->setFlowControl(QSerialPort::FlowControl::NoFlowControl);
    port->setDataBits(QSerialPort::Data8);
    port->setReadBufferSize(bufferSize);

    quint32 row = 0;
    setMaxProgressBar();
    for (const auto baud : m_params.bauds)
    {
        for (const auto parity : m_params.parities)
        {
            for (const auto stopBit : m_params.stopBits)
            {
                port->setBaudRate(baud);
                port->setParity(parity);
                port->setStopBits(stopBit);
                auto openStatus = port->open(QIODevice::ReadWrite);
                for (auto addr = m_params.startAddr; addr <= m_params.endAddr; addr++)
                {
                    // При закрытии окна или остановке поиска через кнопку
                    if (m_stop)
                    {
                        searchFinish(port);
                        return;
                    }

                    m_currentAddress = static_cast<quint8>(addr);
                    createModelItem(row, addr, baud, parity, stopBit);
                    if (openStatus)
                        sendRequest(port);
                    else
                        m_portError = true;
                    updateTable(row);
                    updateProgressBar();
                    row++;

                    // При неожиданной ошибке порта закрываем соединение
                    if (m_portError)
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
    m_stop = true;
    port->deleteLater();
    EMessageBox::information(this, "Сканирование завершено!");
}

void SearchProccessDialog::done(int r)
{
    // Если поиск не закончился
    if (!m_stop)
    {
        m_stop = true;
        auto closeTimer = new QTimer(this);
        closeTimer->setSingleShot(true);
        QObject::connect(closeTimer, &QTimer::timeout, this, [this, r](const auto) { QDialog::done(r); });
        closeTimer->start(m_params.timeout);
    }
    else
        QDialog::done(r);
}

void SearchProccessDialog::showContextMenu(const QPoint &pos)
{
    // Получаем индекс под курсором (pos относительно viewport)
    QModelIndex index = m_tableView->indexAt(pos);

    auto model = m_tableView->model();
    QModelIndex statusIndex = model->index(index.row(), 5);
    QString status = model->data(statusIndex, Qt::DisplayRole).toString();

    QMenu menu(this);
    QAction *addAction = menu.addAction("Добавить в таблицу подключений");
    addAction->setEnabled(
        index.isValid() && (status == StatusIndexRS485::Ok || status == StatusIndexRS485::ResponseError));

    if (menu.exec(m_tableView->viewport()->mapToGlobal(pos)) == addAction)
    {
        addToRS485TableView(index);
    }
}

void SearchProccessDialog::addToRS485TableView(const QModelIndex &sourceIndex)
{
    if (!m_targetDialog)
    {
        qDebug() << "Диалог с соединениями не найден!";
        return;
    }

    QList<QString> connections = m_targetDialog->getConnectionNames();

    if (connections.size() >= MAXREGISTRYINTERFACECOUNT)
    {
        bool ok;
        QString toRemove = QInputDialog::getItem(this, "Лимит подключений",
            "Достигнут лимит (5).\nВыберите подключение для удаления:", connections, 0, false, &ok);

        if (!ok || toRemove.isEmpty())
            return;

        m_targetDialog->removeConnection(toRemove);
    }

    QString connectionName
        = QInputDialog::getText(this, "Имя подключения", "Введите имя для нового соединения:", QLineEdit::Normal, "");

    auto *srcModel = m_tableView->model();
    QMap<QString, QVariant> deviceData;

    deviceData[DeviceDataKeysRS485::Name] = connectionName;
    deviceData[DeviceDataKeysRS485::Port] = srcModel->data(srcModel->index(sourceIndex.row(), 0));
    deviceData[DeviceDataKeysRS485::Address] = srcModel->data(srcModel->index(sourceIndex.row(), 1));
    deviceData[DeviceDataKeysRS485::Baud] = srcModel->data(srcModel->index(sourceIndex.row(), 2));
    deviceData[DeviceDataKeysRS485::Parity] = srcModel->data(srcModel->index(sourceIndex.row(), 3));
    deviceData[DeviceDataKeysRS485::StopBits] = srcModel->data(srcModel->index(sourceIndex.row(), 4));

    m_targetDialog->addConnectionFromSearch(deviceData);
    EMessageBox::information(this, "Устройство успешно добавлено!");

    emit deviceAddedSuccessfully();
    close();
}

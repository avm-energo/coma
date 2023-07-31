#pragma once

#include <QByteArray>
#include <QDialog>
#include <QSerialPort>
#include <vector>

class QTimer;
class QTableView;
class QProgressBar;

struct SearchParams
{
    QString port;
    int timeout = 0;
    int startAddr = 0;
    int endAddr = 0;
    std::vector<qint32> bauds;
    std::vector<QSerialPort::Parity> parities;
    std::vector<QSerialPort::StopBits> stopBits;
};

class SearchProccessDialog : public QDialog
{
private:
    SearchParams params;
    QTimer *timeoutTimer;
    QTableView *tableView;
    QProgressBar *progressBar;

    QByteArray response;
    int expectedResponseSize;

    bool timeout;
    bool responseReceived;
    bool portError;

    /// \brief Функция для построения UI диалогового окна.
    void setupUI();

    /// \brief Функция для обработки ошибок последовательного порта.
    void errorHandler(const QSerialPort::SerialPortError error);
    /// \brief Возвращает запрос BSI по протоколу Modbus в виде массива байт.
    QByteArray createRequest(int address);
    /// \brief Функция для отправки запроса BSI устройству с указанным адресом.
    /// \param port - канал связи с утройством.
    /// \param addr - адрес устройства.
    void sendRequest(QSerialPort *port, int addr);
    /// \brief Вызывается при получении ответа от устройства.
    /// \param port - канал связи с утройством.
    void receiveResponse(QSerialPort *port);
    /// \brief Функция для прекращения процесса поиска устройств.
    /// \details Вызывается, если в ходе выполнения поиска были
    /// обнаружены ошибки, связанные с работой последовательного порта.
    void portErrorFinish(QSerialPort *port);

    void createModelItem(quint32 row, int addr, int baud, QSerialPort::Parity parity, QSerialPort::StopBits stopBit);
    void updateTable(quint32 row);

    void setMaxProgressBar();
    void updateProgressBar();

public:
    SearchProccessDialog(const SearchParams &data, QWidget *parent = nullptr);
    void search();
};

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
    bool timeout;
    bool responseReceived;
    bool portError;

    void setupUI();
    void errorHandler(QSerialPort::SerialPortError error);
    void receiveResponse(QSerialPort *port);
    QByteArray createRequest(int address);
    void createModelItem(int addr, int baud, QSerialPort::Parity parity, QSerialPort::StopBits stopBit);
    void sendRequest(QSerialPort *port, int addr);
    void updateTable(quint32 row);
    void setMaxProgressBar();
    void updateProgressBar();

public:
    SearchProccessDialog(const SearchParams &data, QWidget *parent = nullptr);
    void search();
};

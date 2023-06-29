#pragma once

#include <QByteArray>
#include <QDialog>
#include <QSerialPort>
#include <vector>

class QTimer;

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
    bool timeout;
    bool responseReceived;
    bool portError;

    void setupUI();
    void errorHandler(QSerialPort::SerialPortError error);
    void receiveResponse(QSerialPort *port);
    QByteArray createRequest(int address);
    void createModelItem(quint32 row, int addr, int baud, QSerialPort::Parity parity, QSerialPort::StopBits stopBit);
    void search();
    virtual void showEvent(QShowEvent *event) override;

public:
    SearchProccessDialog(const SearchParams &data, QWidget *parent = nullptr);
};

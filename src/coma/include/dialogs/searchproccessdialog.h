#pragma once

#include <QByteArray>
#include <QDialog>
#include <QSerialPort>
#include <vector>

class QTimer;
class QTableView;
class QProgressBar;
class InterfaceSerialDialog;

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

class SearchProccessDialog final : public QDialog
{
    Q_OBJECT

private:
    SearchParams m_params;
    QTimer *m_timeoutTimer;
    QTableView *m_tableView;
    QProgressBar *m_progressBar;
    InterfaceSerialDialog *m_targetDialog{nullptr};

    QByteArray m_response;
    quint8 m_expectedResponseSize;
    quint8 m_currentAddress;

    bool m_timeout;
    bool m_responseReceived;
    bool m_responseError;
    bool m_portError;
    bool m_stop;

    /// \brief Функция для построения UI диалогового окна.
    void setupUI();

    /// \brief Функция для обработки ошибок последовательного порта.
    void errorHandler(const QSerialPort::SerialPortError error);
    /// \brief Возвращает запрос BSI по протоколу Modbus в виде массива байт.
    QByteArray createRequest();
    /// \brief Функция для отправки запроса BSI устройству с указанным адресом.
    /// \param port - канал связи с утройством.
    /// \param addr - адрес устройства.
    void sendRequest(QSerialPort *port);
    /// \brief Вызывается при получении ответа от устройства.
    /// \param port - канал связи с утройством.
    void receiveResponse(QSerialPort *port);
    /// \brief Анализ ответа от устройства.
    bool analyzeResponse(const QByteArray &actualResponse);
    /// \brief Функция для прекращения процесса поиска устройств.
    /// \details Вызывается, если в ходе выполнения поиска были
    /// обнаружены ошибки, связанные с работой последовательного порта.
    void searchFinish(QSerialPort *port);

    /// \brief Функция для добавления элемента в модель отображения данных.
    void createModelItem(quint32 row, int addr, int baud, QSerialPort::Parity parity, QSerialPort::StopBits stopBit);
    /// \brief Обновление табличного отображения данных на основании внутренних данных класса.
    void updateTable(quint32 row);

    /// \brief Устанавливает максимальное число шагов для прогресс-бара.
    void setMaxProgressBar();
    /// \brief Обновляет состояние прогресс-бара.
    void updateProgressBar();

    void showContextMenu(const QPoint &pos);
    void addToRS485TableView(const QModelIndex &sourceIndex);

public:
    /// \brief Конструктор.
    explicit SearchProccessDialog(const SearchParams &data,
        InterfaceSerialDialog *targetDialog = nullptr,
        QWidget *parent = nullptr);
    /// \brief Функция для запуска поиска устройств.
    void search();

signals:
    void deviceAddedSuccessfully();

private slots:
    virtual void done(int r) override;
};

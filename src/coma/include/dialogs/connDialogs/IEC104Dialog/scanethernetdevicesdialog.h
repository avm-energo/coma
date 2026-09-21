#pragma once

#include <QList>
#include <QWidget>

class QLineEdit;
class QProgressBar;
class QPushButton;
class QStackedWidget;
class InterfaceEthernetDialog;

class ScanEthernetDevicesDialog final : public QWidget
{
    Q_OBJECT

    QList<quint32> m_hosts;
    /// \brief Номер текущего скана. Увеличивается при запуске и при отмене — фоновые задачи
    /// предыдущего скана, завершившиеся уже после этого, распознают себя как устаревшие
    /// по несовпадению захваченного номера с текущим и ничего не делают, кроме самоудаления.
    int m_scanGeneration { 0 };
    /// \brief Число ещё не завершившихся задач пинга текущего скана адресов.
    int m_pendingPings { 0 };

    // ### UI ###
    QStackedWidget *m_stack { nullptr };
    QWidget *m_paramsPage { nullptr };
    QWidget *m_addressScanPage { nullptr };
    QWidget *m_portScanPage { nullptr };

    QLineEdit *m_subnetEdit { nullptr };
    QPushButton *m_scanButton { nullptr };

    QProgressBar *m_addressProgressBar { nullptr };
    QPushButton *m_addressCancelButton { nullptr };

    QProgressBar *m_portProgressBar { nullptr };
    QPushButton *m_portCancelButton { nullptr };
    // ##########

    InterfaceEthernetDialog *m_targetDialog { nullptr };

    void setupUI();
    QWidget *createParamsPage();
    QWidget *createAddressScanPage();
    QWidget *createPortScanPage();

    void runScan();
    void cancelScan();
    /// \brief Возвращает стек на первую страницу (параметры).
    void backToParams();

    void handlePingFinish();
    void handlePortFinish();
    void createPingTask(quint32 ip);
    void createPortTask();

public:
    explicit ScanEthernetDevicesDialog(InterfaceEthernetDialog *targetDialog, QWidget *parent = nullptr);

signals:
    void pingFinished();
    void cancelRequested();
};

#pragma once

#include <QDialog>

class QLabel;
class QProgressBar;
class EProgressIndicator;
class QTimer;
class QPushButton;

class ReconnectDialog : public QDialog
{
    Q_OBJECT
private:
    bool m_success;
    quint32 m_seconds;
    QLabel *m_messageLabel;
    QProgressBar *m_progressBar;
    EProgressIndicator *m_progressIndicator;
    QPushButton *m_cancelButton;
    QTimer *m_closeTimer;

    void setupUI();
    void closeTimerTick();

public:
    explicit ReconnectDialog(QWidget *parent = nullptr);
    virtual void open() override;
    virtual void reject() override;

public slots:
    void reconnectSuccess();

signals:
    void breakConnection();
};

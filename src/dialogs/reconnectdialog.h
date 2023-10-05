#pragma once

#include <QDialog>

class QLabel;
class QProgressBar;
class QProgressIndicator;
class QTimer;

class ReconnectDialog : public QDialog
{
    Q_OBJECT
private:
    bool m_success;
    quint32 m_seconds;
    QLabel *m_messageLabel;
    QProgressBar *m_progressBar;
    QProgressIndicator *m_progressIndicator;
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

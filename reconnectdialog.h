#ifndef RECONNECTDIALOG_H
#define RECONNECTDIALOG_H

#include <QWidget>

class ReconnectDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ReconnectDialog(QWidget *parent=0);

signals:
    void finished();
    void nextturn();
    void timeout();

public slots:
    void run();
    void stop();
    void IncrementTry();

public:

private:
    bool DialogIsAboutToClose;
    int ReconTry;
};

#endif // RECONNECTDIALOG_H

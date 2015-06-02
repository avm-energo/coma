#ifndef RECONNECTDIALOG_H
#define RECONNECTDIALOG_H

#include <QWidget>

#include <QTime>

class ReconnectDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ReconnectDialog(QWidget *parent=0);

signals:
    void finished();
    void nextturn();
    void timeout();
    void changetextlbl(QString);

public slots:
    void run();
    void stop();
    void IncrementTry();

public:

private:
    bool DialogIsAboutToClose;
    int ReconTry;
    QTime tmr;
    QString lbltext;
};

#endif // RECONNECTDIALOG_H

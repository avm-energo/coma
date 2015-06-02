#ifndef OSCDIALOG_H
#define OSCDIALOG_H

#include <QWidget>
#include <QByteArray>

class oscdialog : public QDialog
{
    Q_OBJECT

public:
    explicit oscdialog(QWidget *parent=0);

private:
    void SetupUI();

    QByteArray *OscInfo;

private slots:
    void GetOscInfo();
    void ProcessOscInfo();
};

#endif // OSCDIALOG_H

#ifndef MNKTIME_H
#define MNKTIME_H

#include <QDialog>
#include <QByteArray>
#include <QLabel>
#include <QTime>
#include <QLineEdit>

class MNKTime : public QDialog
{
    Q_OBJECT

public:
    explicit MNKTime(QWidget *parent = nullptr);
    ~MNKTime();

    bool First;
    QTimer *Timer;
    int timeIndex;

private:
    void SetupUI();
    void SetTime(quint32 unixtimestamp);
    void WriteTime(QDateTime &myDateTime);

signals:
   void finished();


public slots:
    void ErrorRead();
    void TimeWritten();

private slots:
    void slot_timeOut();
    void slot2_timeOut();
    void Write_PCDate();
    void Write_Date();
    void Start_Timer(int); //tabClicked
    void Stop_Timer(int);


};

#endif // MNKTIME_H

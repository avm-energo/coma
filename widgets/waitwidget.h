#ifndef WAITWIDGET_H
#define WAITWIDGET_H

#include <QWidget>
#include <QPaintEvent>

class WaitWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WaitWidget(QWidget *parent = 0);
    ~WaitWidget();

    void Start();
    void Stop();

signals:
    void finished();

public slots:
    void SetMessage(QString msg);
    void SetSeconds(quint32 seconds);

private slots:
    void Rotate();

private:
    QString Message;
    quint32 Seconds;
    float gamma;
    float vel1, vel2, vel3, vel4, vel5;

protected:
    void paintEvent(QPaintEvent *e);
    void keyPressEvent(QKeyEvent *e);
};

#endif // WAITWIDGET_H

#ifndef SIGNALCHOOSEWIDGET_H
#define SIGNALCHOOSEWIDGET_H

#include <QLabel>
#include <QMouseEvent>

class SignalChooseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SignalChooseWidget(QStringList &snames, QStringList &discr, QWidget *parent = nullptr);

signals:
    void SignalChoosed(QString signame); // при выборе сигнала - требуется выделить тренд
    void SignalToggled(QString signame,
        bool isChecked); // при включении или отключении \
                                                         // вставить или убрать из осциллограммы

public slots:
    void SetChecked(QString signame, bool checked);

private slots:
    void SignalClicked();
    void SignalChecked(bool isChecked);
};

class MarkSignalWidget : public QLabel
{
    Q_OBJECT
public:
    explicit MarkSignalWidget(const QString &text, QWidget *parent = nullptr);

signals:
    void Clicked();

protected:
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
};
#endif // SIGNALCHOOSEWIDGET_H

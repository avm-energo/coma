#ifndef SIGNALCHOOSEWIDGET_H
#define SIGNALCHOOSEWIDGET_H

#include <QLabel>
#include <QMouseEvent>

class SignalChooseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SignalChooseWidget(const QStringList &snames, const QStringList &discr, QWidget *parent = nullptr);

signals:
    // при выборе сигнала - требуется выделить тренд при включении или отключении вставить или убрать из осциллограммы
    void signalChoosed(QString signame);
    //
    void signalToggled(QString signame, bool isChecked);

public slots:
    void setChecked(QString signame, bool checked);
};

class MarkSignalWidget : public QLabel
{
    Q_OBJECT
public:
    explicit MarkSignalWidget(const QString &text, QWidget *parent = nullptr);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *e) override;
};
#endif // SIGNALCHOOSEWIDGET_H

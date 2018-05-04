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

    enum ww_format
    {
        WW_TIME, // format: mm:ss
        WW_SIMPLE // format: ss
    };

    struct ww_struct
    {
        bool isallowedtostop;
        bool isincrement;
        int format; // ww_format
        quint32 initialseconds; // initial seconds counter
    };

    void Init(ww_struct &ww);
    void Start();
    void Stop();

signals:
    void finished();
    void CountZero(); // signal emits when a number of seconds is down-counted to zero

public slots:
    void SetMessage(QString msg);
    void SetSeconds(quint32 seconds);

private slots:
    void Rotate();
    void UpdateSeconds();

private:
    QString Message;
    quint32 Seconds;
    float gamma;
    float vel1;
    bool IsAllowedToStop, IsIncrement;
    int TimeFormat;
    const QString PressAnyKeyString = "Нажмите Esc, если готовы продолжить";
    int PressAnyKeyStringWidth;

protected:
    void paintEvent(QPaintEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void closeEvent(QCloseEvent *e);
};

#endif // WAITWIDGET_H

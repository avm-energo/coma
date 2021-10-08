#ifndef EPOPUP_H
#define EPOPUP_H

#include <QCloseEvent>
#include <QDialog>
#include <QKeyEvent>
#include <QShowEvent>

class EPopup : public QDialog
{
    Q_OBJECT
public:
    enum MessageTypes
    {
        INFOMESSAGE = 0,
        WARNMESSAGE = 1,
        QUESTMSG = 2,
        ERMESSAGE = 3,
        NEXTMSG = 4
    };
    const QStringList c_captions = { "Информация", "Предупреждение", "Вопрос", "Ошибка", "Далее" };

    EPopup(MessageTypes type, const QString &msg, QWidget *parent = nullptr);

private:
    void aboutToClose();

signals:
    void accepted();
    void cancelled();

private slots:
    void acceptSlot();
    void cancelSlot();

protected:
    void showEvent(QShowEvent *e);
    void closeEvent(QCloseEvent *e);
    void keyPressEvent(QKeyEvent *e);
};

class EPopupWorker
{
public:
    static bool m_result;
    static void information(const QString &msg);
    static bool question(const QString &msg); // yes (1) / no (0)
    static void warning(const QString &msg);
    static void error(const QString &msg);
    static bool next(const QString &msg); // next (1) / cancel (0)
};

#endif // EPOPUP_H

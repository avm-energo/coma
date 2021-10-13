#ifndef EPOPUP_H
#define EPOPUP_H

#include <QCloseEvent>
#include <QDialog>
#include <QKeyEvent>
#include <QShowEvent>
#include <map>

class EPopup : public QDialog
{
    Q_OBJECT
public:
    EPopup(QWidget *parent = nullptr);
    void aboutToClose();

signals:
    void accepted();
    void cancelled();

public slots:
    void acceptSlot();
    void cancelSlot();

protected:
    void showEvent(QShowEvent *e);
    void closeEvent(QCloseEvent *e);
    void keyPressEvent(QKeyEvent *e);
};

class ESimplePopup : public EPopup
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

    ESimplePopup(MessageTypes type, const QString &msg, QWidget *parent = nullptr);
    ESimplePopup(MessageTypes type, QWidget *w, QWidget *parent = nullptr);
};

class EEditablePopup : public EPopup
{
    Q_OBJECT
public:
    EEditablePopup(const QString &caption, QWidget *parent = nullptr);
    void addFloatParameter(const QString &name, float &parameter);
    void execPopup();

private:
    std::map<QString, std::unique_ptr<float>> m_floatParList;
    bool m_result;
    QString caption;

private slots:
    void acceptSlot();
};

class EMessageBox
{
public:
    static bool m_result;
    static void information(const QString &msg);
    static bool question(const QString &msg); // yes (1) / no (0)
    static void warning(const QString &msg);
    static void error(const QString &msg);
    static bool next(const QString &msg); // next (1) / cancel (0)
    static bool next(QWidget *w);         // next (1) / cancel (0)
    static bool editableNext(EEditablePopup *popup);
};

#endif // EPOPUP_H

#ifndef EPOPUP_H
#define EPOPUP_H

#include <QCloseEvent>
#include <QDialog>
#include <QFrame>
#include <QKeyEvent>
#include <QShowEvent>
#include <map>
#include <QMessageBox>

class EPopup : public QDialog
{
    Q_OBJECT
public:
    EPopup(QWidget *parent = nullptr);
    void aboutToClose();
    void Exec();
    void adjustPosition();

signals:
    void accepted();
    void cancelled();
    void closed();

public slots:
    void acceptSlot();
    void cancelSlot();

private:
//    QWidget *m_parent;

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
        NEXTMSG = 4,
        WITHOUTANYBUTTONS = 5
    };
    const QStringList c_captions = { "Информация", "Предупреждение", "Вопрос", "Ошибка", "Далее" };

    ESimplePopup(MessageTypes type, const QString &msg, QWidget *parent = nullptr);
    ESimplePopup(MessageTypes type, QWidget *w, QWidget *parent = nullptr);

private:
    void Create(MessageTypes &type, QWidget *w, QWidget *parent = nullptr);

public slots:
    void acceptSlot();
    void cancelSlot();
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
    void cancelSlot();
};

class EPasswordPopup : public EPopup
{
    Q_OBJECT
public:
    EPasswordPopup(const QString &hash, QWidget *parent);

private:
    QString m_hash;
    bool isAboutToClose;
    bool checkPassword(const QString &psw);

signals:
    void passwordIsCorrect();
    void cancel();

protected:
    void keyPressEvent(QKeyEvent *e);
    void closeEvent(QCloseEvent *e);
};

class EMessageBox
{
public:
    static bool m_result;
    static bool password(QWidget *parent, const QString &hash);
    static void information(QWidget *parent, const QString &msg);
    static bool question(const QString &msg); // yes (1) / no (0)
    static void warning(QWidget *parent, const QString &msg);
    static void error(QWidget *parent, const QString &msg);
    static bool next(QWidget *parent, const QString &msg); // next (1) / cancel (0)
    static bool next(QWidget *parent, QWidget *w);         // next (1) / cancel (0)
    static bool editableNext(EEditablePopup *popup);
    static void infoWithoutButtons(
        QWidget *parent, const QString &msg, int timeout = 1); // timeout to close dialog in seconds
};

#endif // EPOPUP_H

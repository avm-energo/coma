#include "epopup.h"

#include "passwordlineedit.h"
#include "wd_func.h"

#include <QApplication>
#include <QCryptographicHash>
#include <QEventLoop>
#include <QGuiApplication>
#include <QPropertyAnimation>
#include <QScreen>
#include <QTimer>
#include <settings/user_settings.h>

ESimplePopup::ESimplePopup(MessageTypes type, const QString &msg, QWidget *parent) : EPopup(parent)
{
    Create(type, WDFunc::NewLBL2(parent, msg), parent);
}

ESimplePopup::ESimplePopup(MessageTypes type, QWidget *w, QWidget *parent) : EPopup(parent)
{
    Create(type, w, parent);
}

void ESimplePopup::Create(MessageTypes &type, QWidget *w, QWidget *parent)
{
    struct msgsStruct
    {
        QString pxFile;
        QString bgrdColor;
    };

    QMap<MessageTypes, msgsStruct> map = {
        { INFOMESSAGE, { ":/icons/info-hex.svg", "c8fcff" } },
        { WARNMESSAGE, { ":/icons/warn-hex.svg", "ffffc3" } },
        { QUESTMSG, { ":/icons/question-hex.svg", "b5b6ff" } },
        { ERMESSAGE, { ":/icons/err-hex.svg", "ffd4d4" } },
        { NEXTMSG, { ":/icons/next-hex.svg", "d6ffce" } },
        { WITHOUTANYBUTTONS, { ":/icons/ordinary-hex.svg", "f3ffc5" } },
    };
    setAttribute(Qt::WA_DeleteOnClose);
    setStyleSheet("QDialog {background-color: #" + map[type].bgrdColor + ";}");
    w->setStyleSheet("QWidget {background-color: #" + map[type].bgrdColor + ";}");
    if (type < c_captions.size())
        setWindowTitle(c_captions.at(type));
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;

    auto icon = WDFunc::NewIcon(parent, map[type].pxFile);
    icon->setStyleSheet("QWidget {background-color: #" + map[type].bgrdColor + ";}");
    hlyout->addWidget(icon);
    hlyout->addWidget(w);
    lyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addStretch(100);
    if (type == ESimplePopup::QUESTMSG)
    {
        hlyout->addWidget(WDFunc::NewPB(parent, "", "Да", this, &ESimplePopup::acceptSlot));
        hlyout->addStretch(5);
        hlyout->addWidget(WDFunc::NewPB(parent, "", "Нет", this, &ESimplePopup::cancelSlot));
    }
    else if (type == ESimplePopup::NEXTMSG)
    {
        hlyout->addWidget(WDFunc::NewPB(parent, "", "Далее", this, &ESimplePopup::acceptSlot));
        hlyout->addStretch(5);
        hlyout->addWidget(WDFunc::NewPB(parent, "", "Отмена", this, &ESimplePopup::cancelSlot));
    }
    else if (type != ESimplePopup::WITHOUTANYBUTTONS)
        hlyout->addWidget(WDFunc::NewPB(parent, "", "Далее", [&] { this->aboutToClose(); }));
    hlyout->addStretch(100);
    lyout->addLayout(hlyout);
    setLayout(lyout);
    this->adjustSize();
}

void ESimplePopup::acceptSlot()
{
    EPopup::acceptSlot();
}

void ESimplePopup::cancelSlot()
{
    EPopup::cancelSlot();
}

bool EMessageBox::m_result = false;

bool EMessageBox::password(QWidget *parent)
{
    auto &settings = Settings::UserSettings::GetInstance();
    Settings::ScopedSettingsGroup _ { settings, "settings" };
    return EMessageBox::password(parent, settings.get<Settings::PasswordHash>());
}

bool EMessageBox::password(QWidget *parent, const QString &hash)
{
    m_result = false;
    auto popup = new EPasswordPopup(hash, parent);
    QObject::connect(popup, &EPasswordPopup::passwordIsCorrect, [] { m_result = true; });
    popup->exec();
    return m_result;
}

void EMessageBox::information(QWidget *parent, const QString &msg)
{
    m_result = false;
    auto popup = new ESimplePopup(ESimplePopup::INFOMESSAGE, msg, parent);
    popup->exec();
}

bool EMessageBox::question(QWidget *parent, const QString &msg)
{
    m_result = false;
    auto popup = new ESimplePopup(ESimplePopup::QUESTMSG, msg, parent);
    QObject::connect(popup, &ESimplePopup::accepted, [] { m_result = true; });
    QObject::connect(popup, &ESimplePopup::cancelled, [] { m_result = false; });
    popup->exec();
    return m_result;
}

void EMessageBox::warning(QWidget *parent, const QString &msg)
{
    m_result = false;
    auto popup = new ESimplePopup(ESimplePopup::WARNMESSAGE, WDFunc::NewLBL2(parent, msg), parent);
    popup->exec();
}

void EMessageBox::error(QWidget *parent, const QString &msg)
{
    m_result = false;
    auto popup = new ESimplePopup(ESimplePopup::ERMESSAGE, msg, parent);
    popup->setWindowFlag(Qt::WindowStaysOnTopHint, true);
    popup->exec();
}

bool EMessageBox::next(QWidget *parent, const QString &msg)
{
    m_result = false;
    auto popup = new ESimplePopup(ESimplePopup::NEXTMSG, msg, parent);
    QObject::connect(popup, &ESimplePopup::accepted, [&] { m_result = true; });
    QObject::connect(popup, &ESimplePopup::cancelled, [&] { m_result = false; });
    popup->exec();
    return m_result;
}

bool EMessageBox::next(QWidget *parent, QWidget *w)
{
    m_result = false;
    auto popup = new ESimplePopup(ESimplePopup::NEXTMSG, w, parent);
    QObject::connect(popup, &ESimplePopup::accepted, [] { m_result = true; });
    QObject::connect(popup, &ESimplePopup::cancelled, [] { m_result = false; });
    popup->exec();
    return m_result;
}

bool EMessageBox::editableNext(EEditablePopup *popup)
{
    m_result = false;
    popup->execPopup();
    QObject::connect(popup, &EPopup::accepted, [] { m_result = true; });
    QObject::connect(popup, &EPopup::cancelled, [] { m_result = false; });
    return m_result;
}

void EMessageBox::infoWithoutButtons(QWidget *parent, const QString &msg)
{
    auto dlg = new ESimplePopup(ESimplePopup::WITHOUTANYBUTTONS, msg, parent);
    dlg->exec();
}

EPopup::EPopup(QWidget *parent) : QDialog(parent)
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setWindowModality(Qt::WindowModal);
    setStyleSheet("QFrame {border-radius: 10px; border: 3px solid gray;}");
}

void EPopup::aboutToClose()
{
    close();
}

void EPopup::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);
}

void EPopup::closeEvent(QCloseEvent *e)
{
    emit closed();
    QDialog::closeEvent(e);
}

void EPopup::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        cancelSlot();
    if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
        acceptSlot();
}

void EPopup::acceptSlot()
{
    emit accepted();
    aboutToClose();
}

void EPopup::cancelSlot()
{
    emit cancelled();
    aboutToClose();
}

EEditablePopup::EEditablePopup(const QString &caption, QWidget *parent) : EPopup(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    this->caption = caption;
}

void EEditablePopup::addFloatParameter(const QString &name, float *parameter)
{
    m_floatParList[name] = parameter;
}

void EEditablePopup::execPopup()
{
    auto lyout = new QVBoxLayout;
    lyout->addWidget(WDFunc::NewLBL2(this, caption));
    for (std::map<QString, float *>::iterator it = m_floatParList.begin(); it != m_floatParList.end(); ++it)
        lyout->addWidget(WDFunc::NewLBLAndLE(this, it->first, it->first, true));
    auto hlyout = new QHBoxLayout;
    hlyout->addStretch(100);
    hlyout->addWidget(WDFunc::NewPB(this, "", "Далее", this, &EEditablePopup::acceptSlot));
    hlyout->addStretch(5);
    hlyout->addWidget(WDFunc::NewPB(this, "", "Отмена", this, &EEditablePopup::cancelSlot));
    hlyout->addStretch(100);
    lyout->addLayout(hlyout);
    setLayout(lyout);
    this->adjustSize();
    exec();
}

void EEditablePopup::acceptSlot()
{
    for (std::map<QString, float *>::iterator it = m_floatParList.begin(); it != m_floatParList.end(); ++it)
    {
        bool isOk = false;
        float fl;
        fl = WDFunc::LEData(this, it->first).toFloat(&isOk);
        float *tmpf = it->second;
        if (isOk)
            *tmpf = fl;
        else
        {
            EMessageBox::warning(this, "Значение " + it->first + "ошибочно, будет принудительно приравнено нулю");
            *tmpf = 0.0;
        }
    }
    emit accepted();
    aboutToClose();
}

void EEditablePopup::cancelSlot()
{
    EPopup::cancelSlot();
}

void EEditablePopup::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        cancelSlot();
    if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
        acceptSlot();
}

EPasswordPopup::EPasswordPopup(const QString &hash, QWidget *parent) : EPopup(parent)
{
    constexpr auto dialogStyle = "QDialog { background-color: #ffe3f9; }";
    constexpr auto widgetStyle = "QWidget { background-color: #ffe3f9; }";
    isAboutToClose = false;
    m_hash = hash;
    QHBoxLayout *hlyout = new QHBoxLayout;
    auto icon = WDFunc::NewIcon(parent, ":/icons/psw-hex.svg");
    icon->setStyleSheet(widgetStyle);
    hlyout->addWidget(icon);
    auto text = WDFunc::NewLBL2(this, "Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc", "pswlbl");
    text->setStyleSheet(widgetStyle);
    hlyout->addWidget(text);
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addLayout(hlyout);
    vlyout->addWidget(WDFunc::NewPswLE2(this, "pswle", QLineEdit::Password));
    setLayout(vlyout);
    adjustSize();
    setStyleSheet(dialogStyle);
}

bool EPasswordPopup::checkPassword(const QString &password)
{
    QCryptographicHash hasher(QCryptographicHash::Sha3_256);
    hasher.addData(password.toUtf8());
    auto buffer = QString::fromUtf8(hasher.result().toHex());
    return (m_hash == buffer);
}

void EPasswordPopup::keyPressEvent(QKeyEvent *e)
{
    if ((e->modifiers() == Qt::AltModifier) || (e->modifiers() == Qt::ControlModifier))
    {
        qCritical("Ошибка при обработке пароля");
        return;
    }

    if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
    {
        if (checkPassword(WDFunc::LEData(this, "pswle")))
        {
            emit passwordIsCorrect();
            aboutToClose();
        }
        else
        {
            qCritical("Пароль введён неверно");
            EMessageBox::warning(this, "Пароль неверен");
        }
    }
    if ((e->key() == Qt::Key_Escape) && !isAboutToClose)
    {
        isAboutToClose = true;
        EPopup::cancelSlot();
    }
    QDialog::keyPressEvent(e);
}

void EPasswordPopup::closeEvent(QCloseEvent *e)
{
    EPopup::closeEvent(e);
}

#include "epopup.h"

#include "../widgets/passwordlineedit.h"
#include "../widgets/wd_func.h"

#include <QCryptographicHash>
#include <QEventLoop>
#include <QGuiApplication>
#include <QPropertyAnimation>
#include <QScreen>
#include <QTimer>

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
        { INFOMESSAGE, { "images/info-hex.svg", "c8fcff" } },
        { WARNMESSAGE, { "images/warn-hex.svg", "ffffc3" } },
        { QUESTMSG, { "images/question-hex.svg", "b5b6ff" } },
        { ERMESSAGE, { "images/err-hex.svg", "ffd4d4" } },
        { NEXTMSG, { "images/next-hex.svg", "d6ffce" } },
        { WITHOUTANYBUTTONS, { "images/ordinary-hex.svg", "f3ffc5" } },
    };
    setAttribute(Qt::WA_DeleteOnClose);
    setStyleSheet("QFrame {background-color: #" + map[type].bgrdColor + ";}");
    w->setStyleSheet("QWidget {background-color: #" + map[type].bgrdColor + ";}");
    if (type < c_captions.size())
        setWindowTitle(c_captions.at(type));
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL2(parent, "", "", new QPixmap(map[type].pxFile)));
    hlyout->addWidget(w);
    lyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addStretch(100);
    if (type == ESimplePopup::QUESTMSG)
    {
        hlyout->addWidget(WDFunc::NewPB(parent, "", "Ага", this, &ESimplePopup::acceptSlot));
        hlyout->addStretch(5);
        hlyout->addWidget(WDFunc::NewPB(parent, "", "Неа", this, &ESimplePopup::cancelSlot));
    }
    else if (type == ESimplePopup::NEXTMSG)
    {
        hlyout->addWidget(WDFunc::NewPB(parent, "", "Далее", this, &ESimplePopup::acceptSlot));
        hlyout->addStretch(5);
        hlyout->addWidget(WDFunc::NewPB(parent, "", "Отмена", this, &ESimplePopup::cancelSlot));
    }
    else if (type != ESimplePopup::WITHOUTANYBUTTONS)
        hlyout->addWidget(WDFunc::NewPB(parent, "", "Ага", [&] { this->aboutToClose(); }));
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

bool EMessageBox::question(const QString &msg)
{
    m_result = false;
    auto popup = new ESimplePopup(ESimplePopup::QUESTMSG, msg);
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

void EMessageBox::infoWithoutButtons(QWidget *parent, const QString &msg, int timeout)
{
    m_result = false;
    auto dlg = new ESimplePopup(ESimplePopup::WITHOUTANYBUTTONS, msg, parent);
    dlg->show();
    auto tmr = new QTimer(parent);
    tmr->setInterval(timeout * 1000);
    tmr->setSingleShot(true);
    QObject::connect(tmr, &QTimer::timeout, [&] { dlg->close(); });
    QEventLoop loop;
    QObject::connect(dlg, &ESimplePopup::destroyed, &loop, &QEventLoop::quit);
    loop.exec();
}

EPopup::EPopup(QWidget *parent) : QFrame(parent)
{
    Q_UNUSED(parent)
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setWindowModality(Qt::WindowModal);
    QPoint point = (parent == nullptr) ? QPoint(0, 0) : parent->frameGeometry().center();
    move(QGuiApplication::screens().at(0)->geometry().center() - point);
    setStyleSheet("QFrame {border-radius: 10px; border-style: solid; border-width: 2px;}");
}

void EPopup::aboutToClose()
{
    auto anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);
    anim->setDuration(500);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
    QEventLoop loop;
    connect(anim, &QPropertyAnimation::finished, &loop, &QEventLoop::quit);
    loop.exec();
    close();
}

void EPopup::exec()
{
    this->show();
    adjustSize();
    QEventLoop el;
    connect(this, &EPopup::closed, &el, &QEventLoop::quit);
    el.exec();
}

void EPopup::showEvent(QShowEvent *e)
{
    QFrame::showEvent(e);
    auto anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setDuration(750);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void EPopup::closeEvent(QCloseEvent *e)
{
    emit closed();
    QFrame::closeEvent(e);
}

void EPopup::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        cancelSlot();
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

void EEditablePopup::addFloatParameter(const QString &name, float &parameter)
{
    m_floatParList[name] = std::make_unique<float>(parameter);
}

void EEditablePopup::execPopup()
{
    auto lyout = new QVBoxLayout;
    lyout->addWidget(WDFunc::NewLBL2(this, caption));
    for (std::map<QString, std::unique_ptr<float>>::iterator it = m_floatParList.begin(); it != m_floatParList.end();
         ++it)
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
    for (std::map<QString, std::unique_ptr<float>>::iterator it = m_floatParList.begin(); it != m_floatParList.end();
         ++it)
    {
        bool isOk = false;
        float fl;
        fl = WDFunc::LEData(this, it->first).toFloat(&isOk);
        if (isOk)
            *it->second = fl;
        else
        {
            EMessageBox::warning(this, "Значение " + it->first + "ошибочно, будет принудительно приравнено нулю");
            *it->second = 0.0;
        }
        emit accepted();
        aboutToClose();
    }
}

void EEditablePopup::cancelSlot()
{
    EPopup::cancelSlot();
}

EPasswordPopup::EPasswordPopup(const QString &hash, QWidget *parent) : EPopup(parent)
{
    isAboutToClose = false;
    m_hash = hash;
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL2(parent, "", "", new QPixmap("images/psw-hex.svg")));
    hlyout->addWidget(
        WDFunc::NewLBL2(this, "Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc", "pswlbl"));
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addLayout(hlyout);
    vlyout->addWidget(WDFunc::NewPswLE2(this, "pswle", QLineEdit::Password));
    setLayout(vlyout);
    this->adjustSize();
}

bool EPasswordPopup::checkPassword(const QString &psw)
{
    QCryptographicHash hasher(QCryptographicHash::Sha3_256);
    hasher.addData(psw.toUtf8());
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
            EMessageBox::warning(this, "Пароль неверен");
    }
    if ((e->key() == Qt::Key_Escape) && !isAboutToClose)
    {
        isAboutToClose = true;
        emit cancel();
        aboutToClose();
    }
    QFrame::keyPressEvent(e);
}

void EPasswordPopup::closeEvent(QCloseEvent *e)
{
    emit cancel();
    EPopup::closeEvent(e);
}

EMsgBox::EMsgBox(QWidget *parent) : EPopup(parent)
{
}

bool EMsgBox::next()
{
    bool m_result = false;
    QObject::connect(this, &EPopup::accepted, [&] { m_result = true; });
    QObject::connect(this, &EPopup::cancelled, [&] { m_result = false; });
    show();
    return m_result;
}

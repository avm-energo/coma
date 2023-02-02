#include "epopup.h"

#include "../widgets/wd_func.h"
#include "../widgets/passwordlineedit.h"

#include <QCryptographicHash>
#include <QEventLoop>
#include <QPropertyAnimation>
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
    setAttribute(Qt::WA_DeleteOnClose);
    if (type < c_captions.size())
        setWindowTitle(c_captions.at(type));
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(w);
    QHBoxLayout *hlyout = new QHBoxLayout;
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
    auto dlg = new EPasswordPopup(hash, parent);
    QObject::connect(dlg, &EPasswordPopup::passwordIsCorrect, [] {
        m_result = true;
    });
//    QObject::connect(dlg, &EPasswordPopup::cancel, [] {
//        m_result = false;
//    });
    dlg->exec();
    if (m_result == true)
        m_result = true;
    return m_result;
}

void EMessageBox::information(QWidget *parent, const QString &msg)
{
    m_result = false;
    auto dlg = new ESimplePopup(ESimplePopup::INFOMESSAGE, msg, parent);
    dlg->exec();
}

bool EMessageBox::question(const QString &msg)
{
    m_result = false;
    auto dlg = new ESimplePopup(ESimplePopup::QUESTMSG, msg);
    QObject::connect(dlg, &ESimplePopup::accepted, [] { m_result = true; });
    QObject::connect(dlg, &ESimplePopup::cancelled, [] { m_result = false; });
    dlg->exec();
    return m_result;
}

void EMessageBox::warning(QWidget *parent, const QString &msg)
{
    m_result = false;
    auto dlg = new ESimplePopup(ESimplePopup::WARNMESSAGE, WDFunc::NewLBL2(parent, msg), parent);
    dlg->exec();
}

void EMessageBox::error(QWidget *parent, const QString &msg)
{
    m_result = false;
    auto dlg = new ESimplePopup(ESimplePopup::ERMESSAGE, msg, parent);
    dlg->setWindowFlag(Qt::WindowStaysOnTopHint, true);
    dlg->exec();
}

bool EMessageBox::next(QWidget *parent, const QString &msg)
{
    m_result = false;
    auto dlg = new ESimplePopup(ESimplePopup::NEXTMSG, msg, parent);
    QObject::connect(dlg, &ESimplePopup::accepted, [&] { m_result = true; });
    QObject::connect(dlg, &ESimplePopup::cancelled, [&] { m_result = false; });
    dlg->exec();
    return m_result;
}

bool EMessageBox::next(QWidget *parent, QWidget *w)
{
    m_result = false;
    auto dlg = new ESimplePopup(ESimplePopup::NEXTMSG, w, parent);
    QObject::connect(dlg, &ESimplePopup::accepted, [] { m_result = true; });
    QObject::connect(dlg, &ESimplePopup::cancelled, [] { m_result = false; });
    dlg->exec();
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

EPopup::EPopup(QWidget *parent) : QDialog(parent)
{
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

void EPopup::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);
    auto anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setDuration(750);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void EPopup::closeEvent(QCloseEvent *e)
{
    QDialog::closeEvent(e);
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
    // QHBoxLayout *hlyout;
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
    m_hash = hash;
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addWidget(
        WDFunc::NewLBL2(this, "Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc", "pswlbl"));
    vlyout->addWidget(WDFunc::NewPswLE2(this, "pswle", QLineEdit::Password));
    setLayout(vlyout);
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
    if (e->key() == Qt::Key_Escape)
    {
        emit cancel();
        aboutToClose();
    }
    QDialog::keyPressEvent(e);
}

void EPasswordPopup::closeEvent(QCloseEvent *e)
{
    emit cancel();
    e->accept();
}

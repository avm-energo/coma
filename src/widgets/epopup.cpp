#include "epopup.h"

#include "../widgets/wd_func.h"

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

void EMessageBox::information(QWidget *parent, const QString &msg)
{
    m_result = false;
    ESimplePopup *dlg = new ESimplePopup(ESimplePopup::INFOMESSAGE, msg, parent);
    dlg->show();
    QEventLoop loop;
    QObject::connect(dlg, &ESimplePopup::destroyed, &loop, &QEventLoop::quit);
    loop.exec();
}

bool EMessageBox::question(const QString &msg)
{
    m_result = false;
    ESimplePopup *dlg = new ESimplePopup(ESimplePopup::QUESTMSG, msg);
    QObject::connect(dlg, &ESimplePopup::accepted, [] { m_result = true; });
    QObject::connect(dlg, &ESimplePopup::cancelled, [] { m_result = false; });
    dlg->show();
    QEventLoop loop;
    QObject::connect(dlg, &ESimplePopup::destroyed, &loop, &QEventLoop::quit);
    loop.exec();
    return m_result;
}

void EMessageBox::warning(QWidget *parent, const QString &msg)
{
    m_result = false;
    ESimplePopup *dlg = new ESimplePopup(ESimplePopup::WARNMESSAGE, WDFunc::NewLBL2(parent, msg), parent);
    dlg->show();
    QEventLoop loop;
    QObject::connect(dlg, &ESimplePopup::destroyed, &loop, &QEventLoop::quit);
    loop.exec();
}

void EMessageBox::error(QWidget *parent, const QString &msg)
{
    m_result = false;
    ESimplePopup *dlg = new ESimplePopup(ESimplePopup::ERMESSAGE, msg, parent);
    dlg->setWindowFlag(Qt::WindowStaysOnTopHint, true);
    dlg->show();
    QEventLoop loop;
    QObject::connect(dlg, &ESimplePopup::destroyed, &loop, &QEventLoop::quit);
    loop.exec();
}

bool EMessageBox::next(QWidget *parent, const QString &msg)
{
    m_result = false;
    ESimplePopup *dlg = new ESimplePopup(ESimplePopup::NEXTMSG, msg, parent);
    QObject::connect(dlg, &ESimplePopup::accepted, [] { m_result = true; });
    QObject::connect(dlg, &ESimplePopup::cancelled, [] { m_result = false; });
    dlg->show();
    QEventLoop loop;
    QObject::connect(dlg, &ESimplePopup::destroyed, &loop, &QEventLoop::quit);
    loop.exec();
    return m_result;
}

bool EMessageBox::next(QWidget *parent, QWidget *w)
{
    m_result = false;
    ESimplePopup *dlg = new ESimplePopup(ESimplePopup::NEXTMSG, w, parent);
    QObject::connect(dlg, &ESimplePopup::accepted, [] { m_result = true; });
    QObject::connect(dlg, &ESimplePopup::cancelled, [] { m_result = false; });
    dlg->show();
    QEventLoop loop;
    QObject::connect(dlg, &ESimplePopup::destroyed, &loop, &QEventLoop::quit);
    loop.exec();
    return m_result;
}

bool EMessageBox::editableNext(EEditablePopup *popup)
{
    m_result = false;
    popup->execPopup();
    QObject::connect(popup, &EPopup::accepted, [] { m_result = true; });
    QObject::connect(popup, &EPopup::cancelled, [] { m_result = false; });
    QEventLoop loop;
    QObject::connect(popup, &EPopup::destroyed, &loop, &QEventLoop::quit);
    loop.exec();
    return m_result;
}

void EMessageBox::infoWithoutButtons(QWidget *parent, const QString &msg, int timeout)
{
    m_result = false;
    ESimplePopup *dlg = new ESimplePopup(ESimplePopup::WITHOUTANYBUTTONS, msg, parent);
    dlg->show();
    QTimer *tmr = new QTimer();
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
    QPropertyAnimation *anim = new QPropertyAnimation(this, "windowOpacity");
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

    QPropertyAnimation *anim = new QPropertyAnimation(this, "windowOpacity");
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
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(WDFunc::NewLBL2(this, caption));
    QHBoxLayout *hlyout;
    for (std::map<QString, std::unique_ptr<float>>::iterator it = m_floatParList.begin(); it != m_floatParList.end();
         ++it)
        lyout->addWidget(WDFunc::NewLBLAndLE(this, it->first, it->first, true));
    hlyout = new QHBoxLayout;
    hlyout->addStretch(100);
    hlyout->addWidget(WDFunc::NewPB(this, "", "Далее", this, &EEditablePopup::acceptSlot));
    hlyout->addStretch(5);
    hlyout->addWidget(WDFunc::NewPB(this, "", "Отмена", this, &EEditablePopup::cancelSlot));
    hlyout->addStretch(100);
    lyout->addLayout(hlyout);
    setLayout(lyout);
    show();
}

void EEditablePopup::acceptSlot()
{
    for (std::map<QString, std::unique_ptr<float>>::iterator it = m_floatParList.begin(); it != m_floatParList.end();
         ++it)
    {
        bool ok;
        float fl;
        fl = WDFunc::LEData(this, it->first).toFloat(&ok);
        if (ok)
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

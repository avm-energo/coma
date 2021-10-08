#include "epopup.h"

#include "../widgets/wd_func.h"

#include <QEventLoop>
#include <QPropertyAnimation>

EPopup::EPopup(MessageTypes type, const QString &msg, QWidget *parent) : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    if (type < c_captions.size())
        setWindowTitle(c_captions.at(type));
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(WDFunc::NewLBL2(this, msg));
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addStretch(100);
    if (type == EPopup::QUESTMSG)
    {
        hlyout->addWidget(WDFunc::NewPB(this, "", "Ага", this, &EPopup::acceptSlot));
        hlyout->addStretch(5);
        hlyout->addWidget(WDFunc::NewPB(this, "", "Неа", this, &EPopup::cancelSlot));
    }
    else if (type == EPopup::NEXTMSG)
    {
        hlyout->addWidget(WDFunc::NewPB(this, "", "Далее", this, &EPopup::acceptSlot));
        hlyout->addStretch(5);
        hlyout->addWidget(WDFunc::NewPB(this, "", "Отмена", this, &EPopup::cancelSlot));
    }
    else
        hlyout->addWidget(WDFunc::NewPB(this, "", "Ага", [&] { this->aboutToClose(); }));
    hlyout->addStretch(100);
    lyout->addLayout(hlyout);
    setLayout(lyout);
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

void EPopup::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);

    QPropertyAnimation *anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setDuration(1000);
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

bool EPopupWorker::m_result = false;

void EPopupWorker::information(const QString &msg)
{
    m_result = false;
    EPopup *dlg = new EPopup(EPopup::INFOMESSAGE, msg);
    dlg->show();
    QEventLoop loop;
    QObject::connect(dlg, &EPopup::destroyed, &loop, &QEventLoop::quit);
    loop.exec();
}

bool EPopupWorker::question(const QString &msg)
{
    m_result = false;
    EPopup *dlg = new EPopup(EPopup::QUESTMSG, msg);
    QObject::connect(dlg, &EPopup::accepted, [] { m_result = true; });
    QObject::connect(dlg, &EPopup::cancelled, [] { m_result = false; });
    dlg->show();
    QEventLoop loop;
    QObject::connect(dlg, &EPopup::destroyed, &loop, &QEventLoop::quit);
    loop.exec();
    return m_result;
}

void EPopupWorker::warning(const QString &msg)
{
    m_result = false;
    EPopup *dlg = new EPopup(EPopup::WARNMESSAGE, msg);
    dlg->show();
    QEventLoop loop;
    QObject::connect(dlg, &EPopup::destroyed, &loop, &QEventLoop::quit);
    loop.exec();
}

void EPopupWorker::error(const QString &msg)
{
    m_result = false;
    EPopup *dlg = new EPopup(EPopup::ERMESSAGE, msg);
    dlg->show();
    QEventLoop loop;
    QObject::connect(dlg, &EPopup::destroyed, &loop, &QEventLoop::quit);
    loop.exec();
}

bool EPopupWorker::next(const QString &msg)
{
    m_result = false;
    EPopup *dlg = new EPopup(EPopup::NEXTMSG, msg);
    QObject::connect(dlg, &EPopup::accepted, [] { m_result = true; });
    QObject::connect(dlg, &EPopup::cancelled, [] { m_result = false; });
    dlg->show();
    QEventLoop loop;
    QObject::connect(dlg, &EPopup::destroyed, &loop, &QEventLoop::quit);
    loop.exec();
    return m_result;
}

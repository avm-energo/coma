#include <QTime>
#include <QLabel>
#include <QVBoxLayout>
#include <QCoreApplication>
#include <QSplashScreen>

#include "reconnectdialog.h"
#include "publicclass.h"

ReconnectDialog::ReconnectDialog(QWidget *parent) :
    QWidget(parent)
{
    DialogIsAboutToClose = false;
    ReconTry = 0;
}

void ReconnectDialog::run()
{
    lbltext = "Потеряна связь с модулем!\nПопытка восстановления № 0";
    QWidget *dlg = new QWidget;
    QLabel *lbl = new QLabel(lbltext);
    connect(this,SIGNAL(changetextlbl(QString)),lbl,SLOT(setText(QString)));
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(lbl);
    dlg->setLayout(lyout);
    dlg->setVisible(true);
    ReconTry = 1;
    do
    {
        quint32 ThrNumOfMs = CS_MSGTRIG;
        tmr.start();
        quint32 CurTimeElapsed;
        do
        {
            CurTimeElapsed = tmr.elapsed();
            if (CurTimeElapsed > ThrNumOfMs)
            {
                ThrNumOfMs += CS_MSGTRIG;
                lbl->setVisible(!(lbl->isVisible()));
            }
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            if (DialogIsAboutToClose)
            {
                dlg->close();
                delete dlg;
                emit finished();
                return;
            }
        } while (CurTimeElapsed < CS_TIMEOUT);
        emit nextturn();
    } while (1);
    emit finished();
    if (!DialogIsAboutToClose)
        emit timeout(); // если выход по превышению попыток установления соединения
    dlg->setVisible(false);
    delete dlg;
}

void ReconnectDialog::stop()
{
    DialogIsAboutToClose = true;
}

void ReconnectDialog::IncrementTry()
{
    ReconTry++;
    tmr.restart();
    QString tmps = lbltext;
    tmps.chop(1);
    emit changetextlbl(tmps+QString::number(ReconTry));
}

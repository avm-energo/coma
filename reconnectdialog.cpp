#include <QTime>
#include <QLabel>
#include <QVBoxLayout>
#include <QCoreApplication>

#include "reconnectdialog.h"
#include "publicclass.h"

ReconnectDialog::ReconnectDialog(QWidget *parent) :
    QWidget(parent)
{
    DialogIsAboutToClose = false;
}

void ReconnectDialog::run()
{
    QWidget *dlg = new QWidget;
    QLabel *lbl = new QLabel("Потеряна связь с модулем!\nПопытка восстановления № 0");
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(lbl);
    dlg->setLayout(lyout);
    dlg->setVisible(true);
    ReconTry = 0;
    do
    {
        lbl->setText(lbl->text().chop(1)+QString::number(ReconTry));
        quint32 ThrNumOfMs = CS_MSGTRIG;
        QTime tmr;
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
        } while (CurTimeElapsed < CS_TIMEOUT);
    } while ((!DialogIsAboutToClose));
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
}

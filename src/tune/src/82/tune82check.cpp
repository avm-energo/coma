#include "tune/82/tune82check.h"

#include <QDialog>
#include <QEventLoop>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <datablocks/82/bda.h>
#include <gen/files.h>
#include <gen/stdfunc.h>
#include <tune/mip.h>
#include <widgets/epopup.h>
#include <widgets/wd_func.h>

Tune82Check::Tune82Check(int tuneStep, Device::CurrentDevice *device, QWidget *parent)
    : AbstractTuneDialog(tuneStep, device, parent)
{
    setupUI();
}

void Tune82Check::setTuneFunctions()
{
    addTuneFunc("Ввод пароля...", &AbstractTuneDialog::CheckPassword);
    addTuneFunc("Отображение схемы подключения...", &Tune82Check::showScheme);
    addTuneFunc("Проверка связи с МИП...", &Tune82Check::checkMip);
    addTuneFunc("Проверка правильности измерения входных сигналов...", &Tune82Check::check);
}

Error::Msg Tune82Check::showScheme()
{
    QString pmpfile;
    switch (m_typeM) // выводим окно с предупреждением о включении РЕТОМ-а по схеме в зависимости от исполнения
    {
    case Device::MezzanineBoard::MTM_81: // 2t0n
        pmpfile = ":/tunes/tune81.png";
        break;
    case Device::MezzanineBoard::MTM_82:
        pmpfile = ":/tunes/tune82.png";
        break;
    case Device::MezzanineBoard::MTM_83:
        pmpfile = ":/tunes/tune83.png";
        break;
    default:
        EMessageBox::error(this, "Mezzanine board is not one of 81, 82 or 83");
        return Error::Msg::GeneralError;
    }
    QWidget *w = new QWidget(this);
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(WDFunc::NewIcon(this, pmpfile));
    lyout->addWidget(WDFunc::NewLBL2(this, "1. Отключите выходы РЕТОМ;"));
    lyout->addWidget(WDFunc::NewLBL2(w, "2. Соберите схему подключения по вышеприведённой картинке;"));
    lyout->addWidget(WDFunc::NewLBL2(w,
        "3. Задайте на РЕТОМ трёхфазный режим токов и напряжений (Uabc, Iabc) с углами "
        "сдвига по всем фазам 0 град.;"));
    lyout->addWidget(WDFunc::NewLBL2(w, "4. Задайте на РЕТОМ значения напряжений по фазам 60 В;"));
    if (m_typeM != Device::MezzanineBoard::MTM_83)
        lyout->addWidget(WDFunc::NewLBL2(w, "    значения токов по фазам 1 А;"));
    lyout->addWidget(WDFunc::NewLBL2(w, "    частоту 50 Гц."));
    lyout->addWidget(WDFunc::NewLBL2(w, "5. Включите выходы РЕТОМ"));
    w->setLayout(lyout);
    if (!EMessageBox::next(this, w))
        CancelTune();
    return Error::Msg::NoError;
}

Error::Msg Tune82Check::check()
{
    Bda82 *bda = new Bda82(this);
    bda->setup(m_device->getUID(), m_sync);
    bda->readAndUpdate();
#ifndef NO_LIMITS
    const auto inom = config["I2nom"].value<S2::FLOAT_6t>();
    return bda->checkValues(m_typeM, inom);
#elif
    return Error::Msg::NoError;
#endif
}

Error::Msg Tune82Check::checkMip()
{
    Mip *mip = new Mip(true, Device::MezzanineBoard::MTM_82, this);
    const auto inom = config["I2nom"].value<S2::FLOAT_6t>();
    static_assert(inom.size() > 3);
    mip->setNominalCurrent(inom.at(3)); // 2nd currents, phase A
    if (!mip->start())
    {
        EMessageBox::error(this, "Нет соединения с МИП");
        return Error::GeneralError;
    }
    QEventLoop el;
    connect(this, &AbstractTuneDialog::cancelled, mip, &Mip::stop);
    connect(mip, &Mip::finished, &el, &QEventLoop::quit);
    el.exec();
    return mip->check();
}

void Tune82Check::showEvent(QShowEvent *e)
{
    e->accept();
}

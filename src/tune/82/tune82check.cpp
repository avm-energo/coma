#include "tune82check.h"

#include "../../datablocks/kiv/bda.h"
#include "../../gen/files.h"
#include "../../gen/stdfunc.h"
#include "../../interfaces/protocom.h"
#include "../../widgets/epopup.h"
#include "../../widgets/wd_func.h"

#include <QDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

Tune82Check::Tune82Check(ConfigV *config, int tuneStep, Modules::MezzanineBoard type, QWidget *parent)
    : AbstractTuneDialog(config, tuneStep, parent)
{
    m_typeM = type;
    setupUI();
}

void Tune82Check::setTuneFunctions()
{
    addTuneFunc("Ввод пароля...", &AbstractTuneDialog::CheckPassword);
    addTuneFunc("Сохранение текущей конфигурации...", &AbstractTuneDialog::saveWorkConfig);
    addTuneFunc("Отображение схемы подключения...", &Tune82Check::showScheme);
    addTuneFunc("Проверка связи с МИП...", &Tune82Check::checkMip);
    addTuneFunc("Проверка...", &Tune82Check::check);
}

Error::Msg Tune82Check::showScheme()
{
    QString pmpfile;
    switch (m_typeM) // выводим окно с предупреждением о включении РЕТОМ-а по схеме в зависимости от исполнения
    {
    case Modules::MezzanineBoard::MTM_81: // 2t0n
    {
        pmpfile = "images/tune81.png";
        break;
    }
    case Modules::MezzanineBoard::MTM_82:
    {
        pmpfile = "images/tune82.png";
        break;
    }
    case Modules::MezzanineBoard::MTM_83:
    {
        pmpfile = "images/tune83.png";
        break;
    }
    default:
    {
        EMessageBox::error(this, "Mezzanine board is not one of 81, 82 or 83");
        return Error::Msg::GeneralError;
    }
    }
    QWidget *w = new QWidget(this);
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(WDFunc::NewLBL2(this, "", "", new QPixmap(pmpfile)));
    lyout->addWidget(WDFunc::NewLBL2(this, "1. Отключите выходы РЕТОМ;"));
    lyout->addWidget(WDFunc::NewLBL2(w, "2. Соберите схему подключения по вышеприведённой картинке;"));
    lyout->addWidget(WDFunc::NewLBL2(w,
        "3. Задайте на РЕТОМ трёхфазный режим токов и напряжений (Uabc, Iabc) с углами "
        "сдвига по всем фазам 0 град.;"));
    lyout->addWidget(WDFunc::NewLBL2(w, "4. Задайте на РЕТОМ значения напряжений по фазам 60 В;"));
    if (m_typeM != Modules::MezzanineBoard::MTM_83)
        lyout->addWidget(WDFunc::NewLBL2(w, "    Задайте на РЕТОМ значения токов по фазам 1 А;"));
    lyout->addWidget(WDFunc::NewLBL2(w, "5. Включите выходы РЕТОМ"));

    w->setLayout(lyout);
    if (!EMessageBox::next(this, w))
        CancelTune();
    return Error::Msg::NoError;
}

Error::Msg Tune82Check::check()
{
    Bda *bda = new Bda;
    bda->readAndUpdate();
#ifndef NO_LIMITS
    for (int i = 0; i < 3; ++i)
        if (!WDFunc::floatIsWithinLimits("напряжения", bda->data()->Ueff_ADC[i], 2150000.0, 150000.0))
            return Error::Msg::GeneralError;
    for (int i = 3; i < 6; ++i)
        if (!WDFunc::floatIsWithinLimits("тока", bda->data()->Ueff_ADC[i], 1220000.0, 60000.0))
            return Error::Msg::GeneralError;
    if (!WDFunc::floatIsWithinLimits("сопротивления", bda->data()->Pt100, 2123.0, 120.0))
        return Error::Msg::GeneralError;
    if (!WDFunc::floatIsWithinLimits("частоты", bda->data()->Frequency, 51.0, 0.05))
        return Error::Msg::GeneralError;
#endif
    return Error::Msg::NoError;
}

Error::Msg Tune82Check::checkMip()
{
    return Error::Msg::NoError;
}

void Tune82Check::showEvent(QShowEvent *e)
{
    e->accept();
}

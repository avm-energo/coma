#include "tune82checkmip.h"

#include "../module/board.h"
#include "../module/modules.h"
#include "../s2/configv.h"
#include "../tunesequencefile.h"
#include "../widgets/epopup.h"
#include "../widgets/wd_func.h"
#include "../xml/xmlconfigparser.h"

#include <QEventLoop>

Tune82CheckMip::Tune82CheckMip(ConfigV *config, int tuneStep, Modules::MezzanineBoard type, QWidget *parent)
    : AbstractTuneDialog(config, tuneStep, parent)
{
    TuneSequenceFile::init();
    setModuleType(type);
    setupUI();
}

void Tune82CheckMip::setModuleType(Modules::MezzanineBoard type)
{
    if (type == Modules::MezzanineBoard::MTM_81)
        m_moduleType = Mip::AvtukVariants::M81;
    else if (type == Modules::MezzanineBoard::MTM_82)
        m_moduleType = Mip::AvtukVariants::M82;
    else
        m_moduleType = Mip::AvtukVariants::M83;
}

void Tune82CheckMip::setTuneFunctions()
{
    addTuneFunc("1. Отображение схемы подключения...", &Tune82CheckMip::showScheme);
    addTuneFunc("2. Проверка связи с МИП...", &Tune82CheckMip::check);
}

Error::Msg Tune82CheckMip::showScheme()
{
    QString pmpfile;
    switch (m_moduleType) // выводим окно с предупреждением о включении РЕТОМ-а по схеме в зависимости от исполнения
    {
    case Mip::AvtukVariants::M81: // 2t0n
    {
        pmpfile = "images/tune81.png";
        break;
    }
    case Mip::AvtukVariants::M82:
    {
        pmpfile = "images/tune82.png";
        break;
    }
    case Mip::AvtukVariants::M83:
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
    if (m_moduleType != Mip::AvtukVariants::M83)
        lyout->addWidget(WDFunc::NewLBL2(w, "    Задайте на РЕТОМ значения токов по фазам 1 А;"));
    lyout->addWidget(WDFunc::NewLBL2(w, "5. Включите выходы РЕТОМ"));

    w->setLayout(lyout);
    if (!EMessageBox::next(this, w))
        CancelTune();
    return Error::Msg::NoError;
}

Error::Msg Tune82CheckMip::check()
{
    Mip *mip = new Mip(true, m_moduleType);
    mip->setUpdatesEnabled();
    mip->show();
    mip->setNominalCurrent(configV->getRecord(XmlConfigParser::GetIdByName("I2nom")).value<float>());
    mip->start();
    QEventLoop el;
    connect(mip, &Mip::finished, &el, &QEventLoop::quit);
    el.exec();
    return Error::Msg::NoError;
    //    waitNSeconds(5);
    //    mip.stop();
    //    return mip.check();
}

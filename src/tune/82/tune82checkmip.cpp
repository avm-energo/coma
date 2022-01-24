#include "tune82checkmip.h"

#include "../gen/board.h"
#include "../gen/configv.h"
#include "../gen/modules.h"
#include "../widgets/epopup.h"
#include "../widgets/wd_func.h"

Tune82CheckMip::Tune82CheckMip(ConfigV *config, int tuneStep, Modules::MezzanineBoard type, QWidget *parent)
    : AbstractTuneDialog(config, tuneStep, parent)
{
    setModuleType(type);
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

void Tune82CheckMip::setMessages()
{
    m_messages.append("1. Отображение схемы подключения...");
    m_messages.append("2. Проверка связи с МИП...");
}

void Tune82CheckMip::setTuneFunctions()
{
    Error::Msg (AbstractTuneDialog::*func)()
        = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune82CheckMip::showScheme);
    m_tuneFunctions.push_back(func);
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune82CheckMip::check);
    m_tuneFunctions.push_back(func);
}

Error::Msg Tune82CheckMip::showScheme()
{
    QString pmpfile;
    auto &board = Board::GetInstance();
    quint16 typem = board.typeM();
    switch (typem) // выводим окно с предупреждением о включении РЕТОМ-а по схеме в зависимости от исполнения
    {
    case Modules::MezzanineBoard::MTM_81: // 2t0n
    {
        pmpfile = "../tune81.png";
        break;
    }
    case Modules::MezzanineBoard::MTM_82:
    {
        pmpfile = "../tune82.png";
        break;
    }
    case Modules::MezzanineBoard::MTM_83:
    {
        pmpfile = "../tune83.png";
        break;
    }
    default:
        return Error::Msg::GeneralError;
    }
    QWidget *w = new QWidget(this);
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(WDFunc::NewLBL2(w, "", "", new QPixmap(pmpfile)));
    lyout->addWidget(WDFunc::NewLBL2(w, "1. Отключите выходы РЕТОМ;"));
    lyout->addWidget(WDFunc::NewLBL2(w, "2. Соберите схему подключения по вышеприведённой картинке;"));
    lyout->addWidget(WDFunc::NewLBL2(w,
        "3. Задайте на РЕТОМ трёхфазный режим токов и напряжений (Uabc, Iabc) с углами "
        "сдвига по всем фазам 0 град.;"));
    lyout->addWidget(WDFunc::NewLBL2(w, "4. Задайте на РЕТОМ значения напряжений по фазам 60 В;"));
    if (typem != Modules::MezzanineBoard::MTM_83)
        lyout->addWidget(WDFunc::NewLBL2(w, "    Задайте на РЕТОМ значения токов по фазам 1 А;"));
    lyout->addWidget(WDFunc::NewLBL2(w, "5. Включите выходы РЕТОМ"));

    w->setLayout(lyout);
    if (!EMessageBox::next(w))
        CancelTune();
    return Error::Msg::NoError;
}

Error::Msg Tune82CheckMip::check()
{
    Mip mip(false, m_moduleType, this);
    mip.setNominalCurrent(configV->getRecord(BciNumber::I2nom).value<float>());
    mip.start();
    WaitNSeconds(5);
    mip.stop();
    return mip.check();
}

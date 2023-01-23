#include "tunecheckmipdialog.h"

#include "../module/board.h"
#include "../module/modules.h"
#include "../s2/configv.h"
#include "../s2/s2.h"
#include "../widgets/epopup.h"
#include "../widgets/wd_func.h"
#include "../xml/xmlparser/xmlconfigparser.h"
#include "tunesequencefile.h"

#include <QEventLoop>

TuneCheckMipDialog::TuneCheckMipDialog(ConfigV *config, int tuneStep, Modules::MezzanineBoard type, QWidget *parent)
    : AbstractTuneDialog(config, tuneStep, parent)
{
    TuneSequenceFile::init();
    setModuleType(type);
    setupUI();
}

void TuneCheckMipDialog::setModuleType(Modules::MezzanineBoard type)
{
    if (type == Modules::MezzanineBoard::MTM_81)
        m_moduleType = MType::MTM_81;
    else if (type == Modules::MezzanineBoard::MTM_82)
        m_moduleType = MType::MTM_82;
    else
        m_moduleType = MType::MTM_83;
}

void TuneCheckMipDialog::setTuneFunctions()
{
    addTuneFunc("1. Отображение схемы подключения...", &TuneCheckMipDialog::showScheme);
    addTuneFunc("2. Проверка связи с МИП...", &TuneCheckMipDialog::check);
}

Error::Msg TuneCheckMipDialog::showScheme()
{
    QString pmpfile;
    switch (m_moduleType) // выводим окно с предупреждением о включении РЕТОМ-а по схеме в зависимости от исполнения
    {
    case MType::MTM_81: // 2t0n
    {
        pmpfile = "images/tune81.png";
        break;
    }
    case MType::MTM_82:
    {
        pmpfile = "images/tune82.png";
        break;
    }
    case MType::MTM_83:
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
    if (m_moduleType != MType::MTM_83)
        lyout->addWidget(WDFunc::NewLBL2(w, "    Задайте на РЕТОМ значения токов по фазам 1 А;"));
    lyout->addWidget(WDFunc::NewLBL2(w, "5. Включите выходы РЕТОМ"));

    w->setLayout(lyout);
    if (!EMessageBox::next(this, w))
        CancelTune();
    return Error::Msg::NoError;
}

Error::Msg TuneCheckMipDialog::check()
{
    Mip *mip = new Mip(true, m_moduleType);
    mip->takeOneMeasurement(configV->getRecord(S2::GetIdByName("I2nom")).value<DataTypes::FLOAT_6t>().at(3));
    return mip->check();
}

#include "tune82verification.h"

#include "../../datablocks/82/bd1.h"
#include "../../widgets/epopup.h"
#include "../../widgets/wd_func.h"
#include "../mip.h"

#include <gen/stdfunc.h>
#include <interfaces/conn/async_connection.h>

constexpr std::size_t iterCount = 21;
constexpr inline std::array<RetomSettings, iterCount> settings = {
    RetomSettings { 0.0, 60.0, 0.2 },   //
    RetomSettings { 0.0, 60.0, 0.4 },   //
    RetomSettings { 0.0, 60.0, 0.6 },   //
    RetomSettings { 0.0, 60.0, 0.8 },   //
    RetomSettings { 0.0, 60.0, 1.0 },   //
    RetomSettings { 0.0, 60.0, 1.2 },   //
    RetomSettings { 0.0, 60.0, 1.0 },   //
    RetomSettings { 0.0, 60.0, 2.0 },   //
    RetomSettings { 0.0, 60.0, 3.0 },   //
    RetomSettings { 0.0, 60.0, 4.0 },   //
    RetomSettings { 0.0, 60.0, 5.0 },   //
    RetomSettings { 0.0, 60.0, 6.0 },   //
    RetomSettings { 0.0, 72.0, 5.0 },   //
    RetomSettings { 0.0, 48.0, 5.0 },   //
    RetomSettings { 0.0, 36.0, 5.0 },   //
    RetomSettings { 0.0, 24.0, 5.0 },   //
    RetomSettings { 0.0, 12.0, 5.0 },   //
    RetomSettings { 45.0, 60.0, 5.0 },  //
    RetomSettings { 90.0, 60.0, 5.0 },  //
    RetomSettings { 180.0, 60.0, 5.0 }, //
    RetomSettings { 270.0, 60.0, 5.0 }  //
};

struct VerificationOffset
{
    float phiLoad;

    void update(Mip::MipDataStruct &mipData, Bd182::BlockData &deviceData, RetomSettings &retomData) noexcept
    {
        /// TODO
        if (retomData.phiLoad >= 180)
        {
            phiLoad = 360 + deviceData.phi_next_f[3];
            mipData.loadAnglePhase[0] = 360 - mipData.loadAnglePhase[0];
            mipData.loadAnglePhase[1] = 360 - mipData.loadAnglePhase[1];
            mipData.loadAnglePhase[2] = 360 - mipData.loadAnglePhase[2];
        }
        else
            phiLoad = deviceData.phi_next_f[3];
    }
};

Tune82Verification::Tune82Verification(S2::Configuration &config, //
    Modules::MezzanineBoard type, int tuneStep, QWidget *parent)
    : AbstractTuneDialog(config, tuneStep, parent)
    , m_bd1(new Bd182(type, this))
    , m_mip(new Mip(false, type, this))
    , m_typeM(type)
{
}

void Tune82Verification::setTuneFunctions()
{
    addTuneFunc("Ввод пароля...", &AbstractTuneDialog::CheckPassword);
    addTuneFunc("Сохранение текущей конфигурации...", &AbstractTuneDialog::saveWorkConfig);
    addTuneFunc("Поверка...", &Tune82Verification::verification);
}

void Tune82Verification::showRetomDialog(const RetomSettings &retomData)
{

    auto retomDialog = new QDialog(this);
    auto layout = new QVBoxLayout;

    QString tempStr( //
        "Задайте на РЕТОМ трёхфазный режим токов и напряжений "
        "(Uabc, Iabc) с углами нагрузки по всем фазам %1 град. и частотой 51 Гц;");
    auto phiLabel = new QLabel(tempStr.arg(retomData.phiLoad), retomDialog);
    layout->addWidget(phiLabel);

    if (m_typeM != Modules::MezzanineBoard::MTM_81)
    {
        tempStr = "Значения напряжений по фазам %1 В;";
        auto uLabel = new QLabel(tempStr.arg(retomData.voltage), retomDialog);
        layout->addWidget(uLabel);
    }
    if (m_typeM != Modules::MezzanineBoard::MTM_83)
    {
        tempStr = "Значения токов по фазам %1 А;";
        auto iLabel = new QLabel(tempStr.arg(retomData.current), retomDialog);
        layout->addWidget(iLabel);
    }

    auto readyBtn = new QPushButton("Готово", retomDialog);
    connect(readyBtn, &QPushButton::clicked, retomDialog, &QDialog::close);
    layout->addWidget(readyBtn);
    auto cancelBtn = new QPushButton("Отмена", retomDialog);
    connect(cancelBtn, &QPushButton::clicked, this, &Tune82Verification::cancelTune);
    connect(cancelBtn, &QPushButton::clicked, retomDialog, &QDialog::close);
    layout->addWidget(cancelBtn);
    retomDialog->setLayout(layout);
    retomDialog->exec();
}

void Tune82Verification::cancelTune()
{
    StdFunc::Cancel();
    EMessageBox::information(this, "Действие отменено");
}

void Tune82Verification::setCurrentsTo(float value)
{
    S2::FLOAT_6t i2NomConfig { value, value, value, value, value, value };
    config.setRecord("I2nom", i2NomConfig);
    m_async->writeConfiguration(config.toByteArray());
    StdFunc::Wait(500);
}

void Tune82Verification::writeReportData(const QString &name, const QString &value)
{
    m_reportData.insert({ name, value });
}

void Tune82Verification::init()
{
    StdFunc::ClearCancel();
    if (!m_reportData.empty())
        m_reportData.clear();

    writeReportData("Organization", "ООО АСУ-ВЭИ");
    writeReportData("Day", QDateTime::currentDateTime().toString("dd"));
    writeReportData("Month", QDateTime::currentDateTime().toString("MM"));
    writeReportData("Yr", QDateTime::currentDateTime().toString("yy"));
}

Error::Msg Tune82Verification::verification()
{
    float i2nom = 0.0;
    init();

    Mip::MipDataStruct mipData;
    Bd182::BlockData deviceData;
    VerificationOffset offsetData;
    RetomSettings retomData;

    for (std::size_t iter = 0; iter < iterCount; ++iter)
    {
        if (iter == 0)
        {
            i2nom = 1.0;
            setCurrentsTo(i2nom);
        }
        if (iter == 6)
        {
            i2nom = 5.0;
            setCurrentsTo(i2nom);
        }

        retomData = settings[iter];
        showRetomDialog(retomData);
        if (StdFunc::IsCancelled())
            return Error::Msg::GeneralError;

        mipData = m_mip->takeOneMeasurement(i2nom);
        m_bd1->readBlockFromModule();
        deviceData = *(m_bd1->data());
        offsetData.update(mipData, deviceData, retomData);
    }
    return Error::Msg::NoError;
}

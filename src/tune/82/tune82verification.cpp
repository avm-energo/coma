#include "tune82verification.h"

#include "../../widgets/epopup.h"
#include "../../widgets/wd_func.h"
#include "../mip.h"

#include <gen/stdfunc.h>
#include <interfaces/conn/sync_connection.h>

struct RetomSettings
{
    float phiLoad; // phi
    float voltage; // U
    float current; // I
};

constexpr std::size_t phasesCount = 3;
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
    float phiLoad[phasesCount];
    float phiUab, phiUbc, offsetF;
    float offsetU[phasesCount];
    float offsetI[phasesCount];
    float offsetPhiLoad[phasesCount];
    float offsetPhiUab, offsetPhiUbc;

    inline float calculateOffset(const float deviceMeasure, const float mipMeasure) noexcept
    {
        return (100 * ((deviceMeasure / mipMeasure) - 1));
    }

    void update(MipDataStruct &mipData, Bd182::BlockData &deviceData, RetomSettings &retomData) noexcept
    {
        if (retomData.phiLoad >= 180)
        {
            phiLoad[0] = 360 + deviceData.phi_next_f[3];
            mipData.phiLoadPhase[0] = 360 - mipData.phiLoadPhase[0];
            mipData.phiLoadPhase[1] = 360 - mipData.phiLoadPhase[1];
            mipData.phiLoadPhase[2] = 360 - mipData.phiLoadPhase[2];
        }
        else
            phiLoad[0] = deviceData.phi_next_f[3];
        phiLoad[1] = deviceData.phi_next_f[4] - deviceData.phi_next_f[1];
        if (retomData.phiLoad >= 90)
            phiLoad[2] = 360 + deviceData.phi_next_f[5] - deviceData.phi_next_f[2];
        else
            phiLoad[2] = deviceData.phi_next_f[5] - deviceData.phi_next_f[2];
        phiUab = -deviceData.phi_next_f[1];
        phiUbc = 360 - deviceData.phi_next_f[2] + deviceData.phi_next_f[1];

        offsetF = calculateOffset(deviceData.Frequency, mipData.freqUPhase[0]);
        for (std::size_t i = 0; i < phasesCount; ++i)
        {
            offsetU[i] = calculateOffset(deviceData.IUefNat_filt[i], mipData.uPhase[i]);
            offsetI[i] = calculateOffset(deviceData.IUefNat_filt[i + 3], mipData.iPhase[i]);

            // Играемся с углами, чтобы все было в одних значениях и с одинаковыми знаками
            if ((mipData.phiLoadPhase[i] > 0 && phiLoad[i] < 0) || (mipData.phiLoadPhase[i] < 0 && phiLoad[i] > 0))
            {
                offsetPhiLoad[i] = mipData.phiLoadPhase[i] + phiLoad[i];
                mipData.phiLoadPhase[i] = -mipData.phiLoadPhase[i];
            }
            else
                offsetPhiLoad[i] = mipData.phiLoadPhase[i] - phiLoad[i];
        }
        offsetPhiUab = mipData.phiUab - phiUab;
        offsetPhiUbc = mipData.phiUbc - phiUbc;
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
    retomDialog->setAttribute(Qt::WA_DeleteOnClose);
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
    m_sync->writeConfigurationSync(config.toByteArray());
}

void Tune82Verification::writeReportData(const QString &name, const QString &value)
{
    m_reportData.insert({ name, value });
}

void Tune82Verification::writeReportData(const MipDataStruct &mipData, const std::size_t iter)
{
    writeReportData("FreqMIP", QString::number(mipData.freqUPhase[0], 'f', 3));
    writeReportData(QString("UA_MIP.%1").arg(iter), QString::number(mipData.uPhase[0], 'f', 3));
    writeReportData(QString("UB_MIP.%1").arg(iter), QString::number(mipData.uPhase[1], 'f', 3));
    writeReportData(QString("UC_MIP.%1").arg(iter), QString::number(mipData.uPhase[2], 'f', 3));
    writeReportData(QString("IA_MIP.%1").arg(iter), QString::number(mipData.iPhase[0], 'f', 3));
    writeReportData(QString("IB_MIP.%1").arg(iter), QString::number(mipData.iPhase[1], 'f', 3));
    writeReportData(QString("IC_MIP.%1").arg(iter), QString::number(mipData.iPhase[2], 'f', 3));
    writeReportData(QString("PhiLA_MIP.%1").arg(iter), QString::number(mipData.phiLoadPhase[0], 'f', 3));
    writeReportData(QString("PhiLB_MIP.%1").arg(iter), QString::number(mipData.phiLoadPhase[1], 'f', 3));
    writeReportData(QString("PhiLC_MIP.%1").arg(iter), QString::number(mipData.phiLoadPhase[2], 'f', 3));
    writeReportData(QString("PhiUab_MIP.%1").arg(iter), QString::number(mipData.phiUab, 'f', 3));
    writeReportData(QString("PhiUbc_MIP.%1").arg(iter), QString::number(mipData.phiUbc, 'f', 3));
}

void Tune82Verification::writeReportData(const Bd182::BlockData &deviceData, const std::size_t iter)
{
    writeReportData(QString("Freq.%1").arg(iter), QString::number(deviceData.Frequency, 'f', 3));
    writeReportData(QString("UA.%1").arg(iter), QString::number(deviceData.IUefNat_filt[0], 'f', 3));
    writeReportData(QString("UB.%1").arg(iter), QString::number(deviceData.IUefNat_filt[1], 'f', 3));
    writeReportData(QString("UC.%1").arg(iter), QString::number(deviceData.IUefNat_filt[2], 'f', 3));
    writeReportData(QString("IA.%1").arg(iter), QString::number(deviceData.IUefNat_filt[3], 'f', 3));
    writeReportData(QString("IB.%1").arg(iter), QString::number(deviceData.IUefNat_filt[4], 'f', 3));
    writeReportData(QString("IC.%1").arg(iter), QString::number(deviceData.IUefNat_filt[5], 'f', 3));
}

void Tune82Verification::writeReportData(const VerificationOffset &offset, const std::size_t iter)
{
    writeReportData(QString("PhiLA.%1").arg(iter), QString::number(offset.phiLoad[0], 'f', 3));
    writeReportData(QString("PhiLB.%1").arg(iter), QString::number(offset.phiLoad[1], 'f', 3));
    writeReportData(QString("PhiLC.%1").arg(iter), QString::number(offset.phiLoad[2], 'f', 3));
    writeReportData(QString("PhiUab.%1").arg(iter), QString::number(offset.phiUab, 'f', 3));
    writeReportData(QString("PhiUbc.%1").arg(iter), QString::number(offset.phiUbc, 'f', 3));
    writeReportData(QString("OffsetF.%1").arg(iter), QString::number(offset.offsetF, 'f', 3));
    writeReportData(QString("OffsetUA.%1").arg(iter), QString::number(offset.offsetU[0], 'f', 3));
    writeReportData(QString("OffsetUB.%1").arg(iter), QString::number(offset.offsetU[1], 'f', 3));
    writeReportData(QString("OffsetUC.%1").arg(iter), QString::number(offset.offsetU[2], 'f', 3));
    writeReportData(QString("OffsetIA.%1").arg(iter), QString::number(offset.offsetI[0], 'f', 3));
    writeReportData(QString("OffsetIB.%1").arg(iter), QString::number(offset.offsetI[1], 'f', 3));
    writeReportData(QString("OffsetIC.%1").arg(iter), QString::number(offset.offsetI[2], 'f', 3));
    writeReportData(QString("OffsetPhiloadA.%1").arg(iter), QString::number(offset.offsetPhiLoad[0], 'f', 3));
    writeReportData(QString("OffsetPhiloadB.%1").arg(iter), QString::number(offset.offsetPhiLoad[1], 'f', 3));
    writeReportData(QString("OffsetPhiloadC.%1").arg(iter), QString::number(offset.offsetPhiLoad[2], 'f', 3));
    writeReportData(QString("OffsetPhiUAB.%1").arg(iter), QString::number(offset.offsetPhiUab, 'f', 3));
    writeReportData(QString("OffsetPhiUBC.%1").arg(iter), QString::number(offset.offsetPhiUbc, 'f', 3));
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

    MipDataStruct mipData;
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

        writeReportData(mipData, iter);
        writeReportData(deviceData, iter);
        writeReportData(offsetData, iter);
    }
    return Error::Msg::NoError;
}

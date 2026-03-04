#include "tune/82/verification_offset.h"

#include <tune/mip.h>

float calculatePhi(const float phiA, const float phiB) noexcept
{
    float x = 0 - phiA, y = phiB + x;
    if (std::fabs(y) > 180.0)
    {
        float z = 360.0 - std::fabs(y);
        if (z > 0 && phiA > phiB)
            z = z * -1.0;
        return z;
    }
    else
        return (y * -1.0);
}

float VerificationOffset::calculateErrors(const float deviceMeasure, const float mipMeasure, const float range) noexcept
{
    return StdFunc::FloatIsWithinLimits(range, 0.0, 0.1) ? (100 * std::fabs((deviceMeasure / mipMeasure) - 1))
                                                         : (100 * std::fabs((deviceMeasure - mipMeasure) / range));
}

void VerificationOffset::update(
    const MipDataStruct &mipData, const Bd182::BlockData &deviceData, const float rangeU, const float rangeI) noexcept
{
    phiUab = calculatePhi(deviceData.phi_next_f[0], deviceData.phi_next_f[1]);
    phiUbc = calculatePhi(deviceData.phi_next_f[1], deviceData.phi_next_f[2]);
    phiUca = calculatePhi(deviceData.phi_next_f[2], deviceData.phi_next_f[0]);
    errorPhiUab = std::fabs(mipData.phiUab - phiUab);
    errorPhiUbc = std::fabs(mipData.phiUbc - phiUbc);
    offsetF = calculateErrors(deviceData.Frequency, mipData.freqUPhase[0]);
    for (std::size_t i = 0; i < phasesCount; ++i)
    {
        phiLoad[i] = calculatePhi(deviceData.phi_next_f[i], deviceData.phi_next_f[i + 3]);
        errorPhiLoad[i] = std::fabs(mipData.phiLoadPhase[i] - phiLoad[i]);
        errorU[i] = calculateErrors(deviceData.IUefNat_filt[i], mipData.uPhase[i], rangeU);
        errorI[i] = calculateErrors(deviceData.IUefNat_filt[i + 3], mipData.iPhase[i], rangeI);
    }
}

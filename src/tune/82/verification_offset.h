#pragma once

#include "../../datablocks/82/bd1.h"

constexpr std::size_t phasesCount = 3;

struct MipDataStruct;

struct RetomSettings
{
    float phiLoad; // phi
    float voltage; // U
    float current; // I
};

struct VerificationOffset
{
    float phiLoad[phasesCount];
    float phiUab, phiUbc, phiUca, offsetF;
    float offsetU[phasesCount];
    float offsetI[phasesCount];
    float offsetPhiLoad[phasesCount];
    float offsetPhiUab, offsetPhiUbc;

    float calculateOffset(const float deviceMeasure, const float mipMeasure) noexcept;
    float calculatePhi(const float phiA, const float phiB) noexcept;
    void update(MipDataStruct &mipData, Bd182::BlockData &deviceData) noexcept;
};

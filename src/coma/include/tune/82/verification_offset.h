#pragma once

#include <datablocks/82/bd1.h>

constexpr std::size_t phasesCount = 3;

struct MipDataStruct;

struct RetomSettings
{
    float phiLoad; // phi
    float voltage; // U
    float current; // I
};

/// \brief Перевод значений углов из формата блока Bd1 (phi_next_f) в формат МИП-02.
/// \details Используется для перевода углов нагрузки между фазами и углов между
/// фазами напряжения в формат, предоставляемый устройством МИП-02.
float calculatePhi(const float phiA, const float phiB) noexcept;

struct VerificationOffset
{
    float phiLoad[phasesCount];
    float phiUab, phiUbc, phiUca, offsetF;
    float errorU[phasesCount];
    float errorI[phasesCount];
    float errorPhiLoad[phasesCount];
    float errorPhiUab, errorPhiUbc;

    float calculateErrors(const float deviceMeasure, const float mipMeasure, const float range = 0.0) noexcept;
    void update(const MipDataStruct &mipData, const Bd182::BlockData &deviceData, const float rangeU, const float rangeI) noexcept;
};

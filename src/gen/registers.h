#pragma once
#include <array>
//#define INITREG 4000
//#define INITREGCOUNT 11 // 11 floats results in 22 bytes

namespace Regs
{

constexpr uint16_t bsiExtStartReg = 31;
constexpr uint16_t timeReg = 4600;
constexpr uint16_t timeCountRegs = 1;
constexpr uint16_t setInitReg = 900;
constexpr uint16_t clearReg = 905;
constexpr uint16_t bsiStartReg = 1;
constexpr uint16_t bsiEndReg = 15;
constexpr uint16_t bsiCountRegs = 15;
constexpr std::array<uint16_t, bsiCountRegs> bsiReg { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
}

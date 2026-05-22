#pragma once

namespace DeviceDataKeysRS485
{
inline constexpr const char *Name = "name";
inline constexpr const char *Port = "port";
inline constexpr const char *Address = "address";
inline constexpr const char *Baud = "baud";
inline constexpr const char *Parity = "parity";
inline constexpr const char *StopBits = "stopBits";
}

namespace StatusIndexRS485
{
inline constexpr const char *Ok = "Ok";
inline constexpr const char *Error = "Error";
inline constexpr const char *Timeout = "Timeout";
inline constexpr const char *ResponseError = "Response error";
}
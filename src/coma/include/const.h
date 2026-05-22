#pragma once

namespace SerialKeysRS485
{
inline constexpr const char *name = "name";
inline constexpr const char *port = "port";
inline constexpr const char *address = "address";
inline constexpr const char *baud = "baud";
inline constexpr const char *parity = "parity";
inline constexpr const char *stopBits = "stopBits";
}

namespace ParityRS485 {
inline constexpr const char *noParity = "Нет";
inline constexpr const char *oddParity = "Нечет";
inline constexpr const char *evenParity = "Чет";
}

namespace StatusIndexRS485
{
inline constexpr const char *ok = "Ok";
inline constexpr const char *error = "Error";
inline constexpr const char *timeout = "Timeout";
inline constexpr const char *responseError = "Response error";
}
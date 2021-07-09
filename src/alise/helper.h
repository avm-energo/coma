#pragma once
#include <iostream>
#include <time.h>
inline std::ostream &operator<<(std::ostream &os, const timespec &time)
{

    os << time.tv_sec << ":" << time.tv_nsec << std::endl;
    os << long((time.tv_sec * 1000) + (time.tv_nsec / 1.0e6));
    return os;
}

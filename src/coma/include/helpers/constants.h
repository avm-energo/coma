#pragma once

#include <QMap>

namespace Constants
{
enum PathsEnum
{
    FROSYA,

};

const QMap<PathsEnum, QString> Resources { { FROSYA, ":/icons/tnfrosya.svg" } };
}

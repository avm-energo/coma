#pragma once

#include "s2datatypes.h"
#include <avm-gen/datatypes.h>

namespace S2
{
// Forward declarations
class ConfigStorage;
class Configuration;
class DataFactory;

/// \brief Класс, предоставляющий интерфейс для работы с массивами
/// данных в формате S2, конвертированию их из одного формата в другие.
class Util
{
public:
    Util() = default;

    /// \brief Возвращает размер файла, полученного в формате S2.
    [[nodiscard]] std::size_t getFileSize(const QByteArray &s2file) const;
    /// \brief Возвращает массив байт в формате S2B.

    // deprecated part
    static void StoreDataMem(QByteArray &mem, const QList<DataRec> &dr, int fname);
    static bool RestoreData(QByteArray bain, QList<DataRec> &outlist);
    //[[deprecated]] static quint16 GetIdByName(const QString &name);

private:
    static quint32 getTime32() noexcept;
};

} // namespace S2

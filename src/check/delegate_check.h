#ifndef DELEGATE_H
#define DELEGATE_H
#include "../widgets/delegate_common.h"

#include <QStringList>
#include <cstdint>
#include <optional>
#include <variant>

namespace check
{

namespace detail
{

    struct Record
    {
        uint16_t start_addr;
        std::optional<uint16_t> group;
        std::optional<uint16_t> count;
        std::optional<QString> header;
        std::optional<QStringList> desc;
        void validate() const
        {
            assert(count.value() == desc.value().count());
        }
    };

    struct RecordList
    {
        uint16_t group;
        QString header;
        std::vector<Record> records;
    };
}
using itemVariant = std::variant<detail::Record, detail::RecordList>;
using itemVector = std::vector<itemVariant>;
}
#endif // DELEGATE_H

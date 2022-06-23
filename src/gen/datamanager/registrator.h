#ifndef REGISTRATOR_H
#define REGISTRATOR_H

#include "../datatypes.h"

#include <QVariant>
#include <any>
#include <functional>
#include <map>

class Registrator
{
private:
    std::map<std::size_t, std::function<void(QVariant &, std::any &)>> sender_map;
    std::map<std::size_t, std::any> object_map;

public:
    Registrator() = default;

    template <typename T> T &RegistrateType()
    {
        auto hash_type = typeid(T).hash_code();
        auto lambda_sender = [](QVariant &obj, std::any &holder) {
            if (obj.canConvert<T>())
            {
                auto type = obj.value<T>();
                type.SendSignal();
                holder = std::move(type);
            }
        };

        std::function sender(lambda_sender);
        sender_map.insert(hash_type, sender);
        auto any = std::make_any<T>();
        any.emplace();
        object_map.insert(hash_type, any);
        return std::any_cast<T>(any);
    }

    template <typename T> void CallSender(QVariant &data)
    {
        auto hash_type = typeid(T).hash_code();
        auto iter_s = sender_map.find(hash_type);
        auto iter_o = object_map.find(hash_type);
        if (iter_s != sender_map.end() && iter_o != object_map.end())
        {
            auto sender = iter_s->second;
            auto holder = iter_o->second;
            sender(data, holder);
        }
        Q_ASSERT_X(false, "Registrator::CallSender", "Dead sender");
    }
};

#endif // REGISTRATOR_H

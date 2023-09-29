#pragma once

#include <QObject>
#include <gen/std_ext.h>
#include <type_traits>
#include <utility>

namespace detail
{

template <typename T> //
struct slot_trait_impl;

template <typename Ret, typename Class, typename Arg> //
struct slot_trait_impl<Ret (Class::*)(Arg)>
{
    typedef std_ext::remove_cvref_t<Ret> ret_type;
    typedef std_ext::remove_cvref_t<Class> class_type;
    typedef std_ext::remove_cvref_t<Arg> arg_type;
    typedef Arg pure_arg_type;
};

template <typename F, typename Variant, std::size_t I = std::variant_size<Variant>::value>
static constexpr std::pair<bool, std::size_t> is_invocable_with_variant()
{
    if constexpr (I > 0)
    {
        constexpr auto index = I - 1;
        if constexpr (std::is_invocable_v<F, std::variant_alternative_t<index, Variant>>)
            return { true, index };
        else
            return is_invocable_with_variant<F, Variant, I - 1>();
    }
    else
        return { false, -1 };
}

} // namespace detail

namespace Interface
{

template <typename T> //
struct slot_trait : detail::slot_trait_impl<T>
{
};

template <typename T> //
struct slot_trait<T *> : detail::slot_trait_impl<T>
{
};

template <typename T> //
struct slot_trait<T &> : detail::slot_trait_impl<T>
{
};

template <typename T> //
struct slot_trait<const T> : slot_trait<T>
{
};

template <typename Class> //
inline constexpr bool is_qobject = std::is_base_of_v<QObject, std_ext::remove_cvref_t<Class>>;

template <typename F> //
inline constexpr bool is_void_ret_type = std::is_same_v<typename slot_trait<F>::ret_type, void>;

template <typename Class, typename F> //
inline constexpr bool is_same_class
    = std::is_same_v<std_ext::remove_cvref_t<Class>, typename slot_trait<F>::class_type>;

template <typename F, typename Variant, typename Arg = typename slot_trait<F>::arg_type> //
inline constexpr bool is_arg_variant_type = std_ext::is_variant_alternative<Arg, Variant>();

template <typename F, typename Trait = slot_trait<F>> //
inline constexpr bool is_arg_cref = std::is_same_v<typename Trait::pure_arg_type, const typename Trait::arg_type &>;

template <typename Class, typename Slot, typename Variant> //
inline constexpr bool slot_checks = is_qobject<Class> &&is_void_ret_type<Slot> &&is_same_class<Class, Slot>
    &&is_arg_variant_type<Slot, Variant> &&is_arg_cref<Slot>;

template <typename L, typename Variant, std::size_t index = detail::is_invocable_with_variant<L, Variant>().second> //
struct lambda_trait
{
    static constexpr std::size_t variant_index = index;
    typedef std::variant_alternative_t<index, Variant> arg_type;
};

template <typename L, typename Variant> //
inline constexpr bool lambda_is_invocable = detail::is_invocable_with_variant<L, Variant>().first;

template <typename Class, typename L, typename Variant> //
inline constexpr bool lambda_checks = is_qobject<Class> &&lambda_is_invocable<L, Variant>;

using MetaConnection = QMetaObject::Connection;

} // namespace Interface

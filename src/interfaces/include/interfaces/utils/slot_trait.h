#pragma once

#include <QObject>
#include <gen/std_ext.h>
#include <type_traits>

// Default calling convention is CDECL
#if !defined(_M_X64) && !defined(__amd64__)
#if defined(__GNUC__)
#define CCDECL __attribute__((cdecl))
#elif defined(_MSC_VER)
#define CCDECL __cdecl
#endif
#else // #if !defined(_M_X64) && !defined(__amd64__)
#define CCDECL
#endif

namespace detail
{

template <typename T> //
struct slot_trait_impl;

#if defined(CCDECL)
template <typename Ret, typename Class, typename Arg> //
struct slot_trait_impl<Ret (CCDECL Class::*)(Arg)>
{
    typedef std_ext::remove_cvref_t<Ret> ret_type;
    typedef std_ext::remove_cvref_t<Class> class_type;
    typedef std_ext::remove_cvref_t<Arg> arg_type;
};
#endif

template <typename F, typename Variant, std::size_t I = std::variant_size<Variant>::value>
static constexpr bool is_invocable_with_variant()
{
    if constexpr (I > 0)
    {
        constexpr auto index = I - 1;
        if constexpr (std::is_invocable_v<F, std::variant_alternative_t<index, Variant>>)
            return true;
        else
            return is_invocable_with_variant<F, Variant, I - 1>();
    }
    else
        return false;
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

template <typename Class, typename F, typename Variant>
inline constexpr bool slot_checks
    = is_qobject<Class> &&is_void_ret_type<F> &&is_same_class<Class, F> &&is_arg_variant_type<F, Variant>;

} // namespace Interface

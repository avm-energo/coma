#pragma once

#include <QObject>
#include <gen/std_ext.h>
#include <type_traits>

// Default calling convention is CDECL
#if !defined(_M_X64) && !defined(__amd64__)
#if defined(__GNUC__)
#define CCDECL __attribute__((cdecl))
#define CFASTCALL __attribute__((fastcall))
#define CSTDCALL __attribute__((stdcall))
#define CTHISCALL __attribute__((thiscall))
#elif defined(_MSC_VER)
#define CCDECL __cdecl
#define CFASTCALL __fastcall
#define CSTDCALL __stdcall
#define CTHISCALL __thiscall
#define CVECTORCALL __vectorcall
#endif
#else // #if !defined(_M_X64) && !defined(__amd64__)
#define CCDECL
#ifdef _MSC_VER
#define CVECTORCALL __vectorcall
#endif // #ifdef _MSC_VER
#endif

namespace detail
{

template <typename Ret, typename Class, typename Arg> //
struct base_slot_trait
{
    typedef std_ext::remove_cvref_t<Ret> ret_type;
    typedef std_ext::remove_cvref_t<Class> class_type;
    typedef std_ext::remove_cvref_t<Arg> arg_type;
    typedef Arg pure_arg_type;
};

template <typename T> //
struct slot_trait_impl;

#if defined(CCDECL)
template <typename Ret, typename Class, typename Arg> //
struct slot_trait_impl<Ret (CCDECL Class::*)(Arg)> : base_slot_trait<Ret, Class, Arg>
{
};
#endif // defined(CCDECL)

#if defined(CFASTCALL)
template <typename Ret, typename Class, typename Arg> //
struct slot_trait_impl<Ret (CFASTCALL Class::*)(Arg)> : base_slot_trait<Ret, Class, Arg>
{
};
#endif // defined(CFASTCALL)

#if defined(CSTDCALL)
template <typename Ret, typename Class, typename Arg> //
struct slot_trait_impl<Ret (CSTDCALL Class::*)(Arg)> : base_slot_trait<Ret, Class, Arg>
{
};
#endif // defined(CSTDCALL)

#if defined(CTHISCALL)
template <typename Ret, typename Class, typename Arg> //
struct slot_trait_impl<Ret (CTHISCALL Class::*)(Arg)> : base_slot_trait<Ret, Class, Arg>
{
};
#endif // defined(CTHISCALL)

#if defined(CVECTORCALL)
template <typename Ret, typename Class, typename Arg> //
struct slot_trait_impl<Ret (CVECTORCALL Class::*)(Arg)> : base_slot_trait<Ret, Class, Arg>
{
};
#endif // defined(CVECTORCALL)

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

template <typename F, typename Trait = slot_trait<F>> //
inline constexpr bool is_arg_cref = std::is_same_v<typename Trait::pure_arg_type, const typename Trait::arg_type &>;

template <typename Class, typename Slot, typename Variant> //
inline constexpr bool slot_checks = is_qobject<Class> &&is_void_ret_type<Slot> &&is_same_class<Class, Slot>
    &&is_arg_variant_type<Slot, Variant> &&is_arg_cref<Slot>;

template <typename Class, typename L, typename Variant> //
inline constexpr bool lambda_checks = is_qobject<Class> &&detail::is_invocable_with_variant<L, Variant>();

} // namespace Interface

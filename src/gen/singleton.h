#pragma once
#include <memory>

/*! \brief Basic class for singleton pattern designed classes.
 *
 *  ## See also:
 *  - [Modern C++ Singleton Template](https://codereview.stackexchange.com/questions/173929/modern-c-singleton-template) \emoji smile_cat
 */
template <typename T> class Singleton
{
public:
    static T &GetInstance();
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton) = delete;

protected:
    /// \brief The token empty helper struct.
    struct token
    {
    };

    Singleton()
    {
    }
};

/// \brief Returns instance of singleton class.
template <typename T> T &Singleton<T>::GetInstance()
{
    static const std::unique_ptr<T> instance { new T { token {} } };
    return *instance;
}

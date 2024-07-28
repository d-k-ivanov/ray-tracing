#pragma once

#include <sstream>
#include <string>

namespace Utils
{

namespace Impl
{

struct EmptyString
{
    operator const std::string&() const
    {
        static const std::string EmptyStringLiteral;
        return EmptyStringLiteral;
    }

    operator const char*() const
    {
        return "";
    }
};

template <typename T>
struct CanonicalizeStrTypes
{
    using StringType = const T&;
};

template <size_t N>
struct CanonicalizeStrTypes<char[N]>
{
    using StringType = const char*;
};

inline std::ostream& GetString(std::ostream& ss)
{
    return ss;
}

template <typename T>
std::ostream& GetString(std::ostream& ss, const T& t)
{
    ss << t;
    return ss;
}

template <>
inline std::ostream& GetString<EmptyString>(
    std::ostream& ss,
    const EmptyString&)
{
    return ss;
}

template <typename T, typename... Args>
std::ostream& GetString(std::ostream& ss, const T& t, const Args&... args)
{
    return GetString(GetString(ss, t), args...);
}

template <typename... Args>
struct StringWrapper final
{
    static std::string Call(const Args&... args)
    {
        std::ostringstream ss;
        GetString(ss, args...);
        return ss.str();
    }
};

template <>
struct StringWrapper<> final
{
    static EmptyString Call()
    {
        return {};
    }
};

}    // namespace Impl

template <typename... Args>
std::string ConcatenateStrings(const Args&... args)
{
    return Impl::StringWrapper<typename Impl::CanonicalizeStrTypes<Args>::StringType...>::Call(args...);
}

}    // namespace Utils

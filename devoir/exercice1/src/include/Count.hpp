#ifndef COUNT_HPP
#define COUNT_HPP

#include <type_traits>

template <typename T, T val, T... list>
struct Count;

// Case where the list is empty and it returns 0
template <typename T, T val>
struct Count<T, val> {
    enum : std::size_t { value = 0 };
};

// Case where the list is not empty
// Case Recursive where it counts consecutive occurrences of val
template <typename T, T val, T first, T... rest>
struct Count<T, val, first, rest...> {
    enum : std::size_t {
        value = (first == val ? 1 + Count<T, val, rest...>::value : 0)
    };
};

#endif // COUNT_HPP
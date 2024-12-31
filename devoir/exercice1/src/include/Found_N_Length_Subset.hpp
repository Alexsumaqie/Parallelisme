#ifndef FOUND_N_LENGTH_SUBSET_HPP
#define FOUND_N_LENGTH_SUBSET_HPP

#include <stddef.h>
#include "Count.hpp"

template <typename T, std::size_t n, T... list>
struct Found_N_Length_Subset;

// Base case: if n is 0, Yes is 1 (a zero-length subset always exists)
template <typename T, std::size_t n>
struct Found_N_Length_Subset<T, n> {
    enum : int { Yes = (n == 0) ? 1 : 0 };
};

// Recursive case: checks if there are `n` occurrences of any element in the list
template <typename T, std::size_t n, T first, T... rest>
struct Found_N_Length_Subset<T, n, first, rest...> {
    enum : int {
        Yes = (Count<T, first, first, rest...>::value >= n) ? 1 :
              Found_N_Length_Subset<T, n, rest...>::Yes
    };
};

// Edge case: fewer elements remain than required
template <typename T, std::size_t n, T first>
struct Found_N_Length_Subset<T, n, first> {
    enum : int { Yes = n == 1 };
};

#endif // FOUND_N_LENGTH_SUBSET_HPP
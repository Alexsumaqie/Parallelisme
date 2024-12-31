#ifndef LONGEST_SUBSET_HPP
#define LONGEST_SUBSET_HPP

#include <cstddef>

template <typename T, T... list>
struct Longest_Subset;

// Base case: empty list, longest sequence length is 0
template <typename T>
struct Longest_Subset<T> {
    enum : std::size_t { value = 0 };
};

// Recursive case: finds the longest run of identical consecutive elements
template <typename T, T first, T second, T... rest>
struct Longest_Subset<T, first, second, rest...> {
    enum : std::size_t {
        current_run = (first == second) ? 1 + Longest_Subset<T, second, rest...>::current_run : 1,
        max_run_rest = Longest_Subset<T, second, rest...>::value,
        value = (current_run > max_run_rest) ? current_run : max_run_rest
    };
};

// Special case: single remaining element
template <typename T, T single>
struct Longest_Subset<T, single> {
    enum : std::size_t {
        value = 1,
        current_run = 1
    };
};

#endif // LONGEST_SUBSET_HPP
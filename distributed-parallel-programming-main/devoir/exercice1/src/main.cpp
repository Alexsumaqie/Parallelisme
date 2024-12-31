#include <iostream>
#include "Count.hpp"
#include "Found_N_Length_Subset.hpp"
#include "Longest_Subset.hpp"

int main() {
    std::cout << "== [Count test begin] ==" << std::endl;
    std::cout << "\tCount<int, 5>::value = " << Count<int, 5>::value << std::endl;
    std::cout << "\tCount<char, 'a', 'a'>::value = " << Count<char, 'a', 'a'>::value << std::endl;
    std::cout << "\tCount<int, 5, 5, 5, 6>::value = " << Count<int, 5, 5, 5, 6>::value << std::endl;
    std::cout << "== [Count test end] ==" << std::endl;

    std::cout << "== [Found_N_Length_Subset test begin] ==" << std::endl;
    std::cout << "\tFound_N_Length_Subset<int, 1, 3>::Yes = " << Found_N_Length_Subset<int, 1, 3>::Yes << std::endl;
    std::cout << "\tFound_N_Length_Subset<int, 0>::Yes = " << Found_N_Length_Subset<int, 0>::Yes << std::endl;
    std::cout << "\tFound_N_Length_Subset<char, 2, 'a', 'b', 'a'>::Yes = " << Found_N_Length_Subset<char, 2, 'a', 'b', 'a'>::Yes << std::endl;
    std::cout << "\tFound_N_Length_Subset<char, 2, 'a', 'b', 'a', 'a'>::Yes = " << Found_N_Length_Subset<char, 2, 'a', 'b', 'a', 'a'>::Yes << std::endl;
    std::cout << "== [Found_N_Length_Subset test end] ==" << std::endl;

    std::cout << "== [Longest_Subset test begin] ==" << std::endl;
    std::cout << "\tLongest_Subset<int>::value = " << Longest_Subset<int>::value << std::endl;
    std::cout << "\tLongest_Subset<char, 'a'>::value = " << Longest_Subset<char, 'a'>::value << std::endl;
    std::cout << "\tLongest_Subset<int, 0, 1, 0, 0, 1, 1, 1, 0>::value = " << Longest_Subset<int, 0, 1, 0, 0, 1, 1, 1, 0>::value << std::endl;
    std::cout << "== [Longest_Subset test end] ==" << std::endl;

    return EXIT_SUCCESS;
}
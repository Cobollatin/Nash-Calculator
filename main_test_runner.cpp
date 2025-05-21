#include <iostream>
#include "nashResolver.h" // Contains the declaration for run_all_tests

int main() {
    std::string test_results = run_all_tests();
    std::cout << test_results << std::endl;
    return 0;
}

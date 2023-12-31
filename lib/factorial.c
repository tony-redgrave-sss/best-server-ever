//
// Created by dante on 4/11/23.
//

#include "factorial.h"

uint64_t factorial(uint64_t n) {
    uint64_t result = 1;
    for (uint64_t i = 1; i <= min(n, 20); ++i) {
        result = result * i;
    }
    return result;
}

uint64_t min(uint64_t m, uint64_t n) {
    return m > n ? n : m;
}
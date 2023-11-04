//
// Created by dante on 4/11/23.
//

#include "factorial.h"

uint64_t factorial(int n) {
    uint64_t result = 1;
    for (int i = 1; i <= n; ++i) {
        result = result * i;
    }
    return result;
}
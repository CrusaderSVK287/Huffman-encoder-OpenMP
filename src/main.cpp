#include <iostream>
#include <omp.h>

int main() {

#ifdef _SERIAL
    std::cout << "Warning, running in serial mode" << std::endl;
#endif 

    const int SIZE = 100000;
    int arr[SIZE];
    long long sum = 0;

    // Initialize array with values
    for (int i = 0; i < SIZE; i++) {
        arr[i] = i + 1;
    }

    // Parallel sum calculation
#ifndef _SERIAL
    #pragma omp parallel for reduction(+:sum)
#endif
    for (int i = 0; i < SIZE; i++) {
        sum += arr[i];
    }

    std::cout << "Sum: " << sum << std::endl;
    return 0;
}


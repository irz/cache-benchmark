// Generate matrix data to test cache-aware multiplication

// Copyright (c) 2025 irz
// SPDX-License-Identifier: MIT
// See LICENSE.md for full license text.

#include <iostream>
#include <fstream>
#include <random>

void generate(std::ofstream &out, int size) {
    out << size;
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> dist(0.0, 10.0);

    for (int i = 0; i < size * size; ++i) {
        out << "," << dist(rng);
    }
    out << "\n";
}

// Generate matrix pairs for testing
int main(int argc, char* argv[])
{
    if (!argv[1] || argc != 2)
    {
        std::cerr << "Use: " << argv[0] << " output.txt\n";
        return 1;        
    }    
    std::ofstream file(argv[1]);
    if (!file) {
        std::cerr << "Error: Could not open output file\n";
        return 1;
    }

    int sizes[] = {400, 1000, 2000};

    for (int s : sizes) {
        generate(file, s); // first matrix
        generate(file, s); // second matrix
    }

    std::cout << argv[1] << " generated successfully.\n";
    return 0;
}

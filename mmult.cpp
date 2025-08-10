// Code to benchmark naive vs blocking matrix multiplication 

// Copyright (c) 2025 irz
// SPDX-License-Identifier: MIT
// See LICENSE.md for full license text.

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <chrono>

// Read square matrix description of row-major format: 
// "n(size), a00, a01, ... an0, an1, .. ann"
std::vector<double> readMatrixLine(const std::string &line, int &size)
{
    std::stringstream ss(line);
    std::string token;

    // Read size
    if (!std::getline(ss, token, ','))
    {
        throw std::runtime_error("missing matrix size");
    }
    size = std::stoi(token);

    std::vector<double> matrix(size * size);

    // Read elements
    for (int i = 0; i < size * size; ++i)
    {
        if (!std::getline(ss, token, ','))
        {
            throw std::runtime_error("missing matrix values");
        }
        matrix[i] = std::stod(token);
    }

    return matrix;
}

// Multiply two square matrices stored in flat vectors ("naive" approach)
std::vector<double> multiplyMatrices(const std::vector<double> &A, 
                                     const std::vector<double> &B, 
                                     const int n) 
{
    std::vector<double> C(n * n, 0.0);

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            double sum = .0;
            for (int k = 0; k < n; ++k)
            {
                sum += A[i * n + k] * B[k * n + j];
            }

            C[i * n + j] = sum;
        }
    }
    return C;
}


// Multiply two square matrices stored in flat vectors using blocking approach
std::vector<double> multiplyMatricesBlocked(const std::vector<double> &A, 
                                     const std::vector<double> &B, 
                                     const int n, const int blockSize) 
{
    // Block size should be multiple of matrix size
    if (n % blockSize)
    {
        throw std::runtime_error("invalid block size");
    }

    std::vector<double> C(n * n, 0.0);

    for (int jj = 0; jj < n; jj += blockSize)
    {
        for (int kk = 0; kk < n; kk += blockSize)
        {
            // Blocking rows seems to give little benefit
            // as it's already arrange in cache-friendly way
            for (int i = 0; i < n; ++i)
            {            
                for (int j = jj; j < jj + blockSize; ++j)
                {
                    double sum = C[i * n + j];
                    for (int k = kk; k < kk + blockSize; ++k)
                    {
                        sum += A[i * n + k] * B[k * n + j];
                    }
                    C[i * n + j] = sum;
                }
            }
        }
    }

    return C;
}

void printMatrix(const std::vector<double> &M, int n) {
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            std::cout << std::setprecision(10) << M[i * n + j] << " ";
        }
        std::cout << "\n";
    }
}

int main(int argc, char* argv[]) {

    if (!argv[1] || argc < 2)
    {
        std::cerr << "Use: " << argv[0] << " input.txt {blockSize}\nWhere input.txt format: \"N, a00, a01, ... aN0, aN1, .. aNN\"\n";
        return 1;        
    }

    std::ifstream infile(argv[1]);
    if (!infile)
    {
        std::cerr << "error: could not open: \n" << argv[1];
        return 1;
    }

    std::string line;
    std::vector<std::vector<double>> matrices;
    std::vector<int> sizes;

    while (std::getline(infile, line))
    {
        if (!line.empty())
        {
            int size;
            matrices.push_back(readMatrixLine(line, size));
            sizes.push_back(size);
        }
    }

    if (matrices.size() < 2) {
        std::cerr << "error: need at least two matrices\n";
        return 1;
    }
    if (sizes[0] != sizes[1]) {
        std::cerr << "error: size mismatch\n";
        return 1;
    }

    int blockSize = 0;

    // If block size is provided use it for blocking multiplication later
    if (argc == 3)
    {
        blockSize = std::stoi(argv[2]);
    }

    for (size_t i = 0; i < matrices.size(); i += 2)
    {
        int n = sizes[i];
        if (sizes[i] != sizes[i + 1])
        {
            std::cerr << "size mismatch " << i/2 << "\n";
            continue;
        }

        const int runs = 10;
        double totalTime = .0;
        for (int r = 0; r < runs; ++r)
        {
            auto start = std::chrono::high_resolution_clock::now();

            if (blockSize)
            {
                auto C = multiplyMatricesBlocked(matrices[i], matrices[i + 1], n, blockSize);
            }
            else
            {
                auto C = multiplyMatrices(matrices[i], matrices[i + 1], n);
            }
            
            auto end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double> elapsed = end - start;
            totalTime += elapsed.count();
        }

        std::cout << "size: " << n << "x" << n  << " | avg. time: " << (totalTime / runs) << " seconds\n";
    }

    return 0;
}

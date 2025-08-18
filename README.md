# cache-benchmark
Basic benchmark using cache-aware matrix multiplication.

Multiply 2 square matrices using 'naive' or cache-aware blocked approach.
Use generate to create sets of pairs of square matrices.

Compile:
```
g++ -O3 mmult.cpp -o mmult
g++ -O3 generate.cpp -o generate
```
Generate input matrices (sizes are 400, 1000, 2000 by default):
```
./generate input.txt
```
Non-blocked matrix multiplication:
```
./mmult input.txt
```
Blocked multiplication:
```
./mmult input.txt {blockSize}
```

Results example on x64:

|Matrix size	|Naive exucution time, s	|Blocking execution time, s	|Speedup, x1|
|---|---|---|---|
|400	|0.073	|0.046	|1.59|
|1000	|3.503	|0.75	|4.67|
|2000	|92.7	|6.274	|14.78|

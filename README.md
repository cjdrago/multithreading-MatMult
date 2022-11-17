
# Parallel Matrix Multiplication

In this project, you will implement a parallel version of matrix multiplication, 
called `pmm`.

## Overview

To learn how to perform a matrix multiplication or how to implement a parallel 
version of matrix multiplication, please refer to the project pdf file.

Your parallel matrix multiplication (`pmm`) will use POSIX threads (pthreads) to 
accelerate the matrix multiplication. It will take two files as inputs, and 
output a matrix product; the general usage from the command line will be as 
follows:

```Shell
[you@machine] ./pmm < matrix.txt > product.txt
```

## Considerations

Doing so effectively and with high performance will require you to address (at
least) the following issues:

- **How to parallelize the matrix multiplication.** Parallelizing the matrix 
    multiplication requires you to think about how to partition the matrix 
    multiplication, so that the computing resources can be fully used. Following 
    the mentioned two directions, you can design your own partition schemes. The 
    input matrix varies from different size, different dense/sparse degree. Your 
    schemes should take all those factors into consideration. 

- **How to determine the number of threads to create.** On Linux, the 
    determination of the number of threads may refer to some interfaces like 
    `get_nprocs()` and `get_nprocs_conf()`; You are suggested to read the man 
    pages for more details. Then, you are required to create an appropriate 
    number of threads to match the number of CPUs available on whichever system 
    your program is running.

- **How to balance the efficiency of different threads.** During your 
    implementations, you need to think about what works can be done in parallel, 
    and what works must be done serially by a single thread. For example, in the 
    second direction, each sub-matrix product can be computed in parallel, but 
    the sum of them needs to be done in serial. Another interesting issue is 
    that what happens if one thread runs much slower than another? Does the 
    partition scheme give less work to faster threads? This issue is often 
    referred to as the _straggler problem_.

## Grading

Your code should compile (and should be compiled) with the following flags:
`-Wall -Werror -pthread -O`. The last one is important: it turns on the
optimizer! In fact, for fun, try timing your code with and without `-O` and
marvel at the difference.

Your code will first be measured for correctness, ensuring that it zips input
files correctly.

If you pass the correctness tests, your code will be tested for performance..

## Acknowledgements 

This project is taken (and slightly modified) from the [OSTEP projects](https://github.com/remzi-arpacidusseau/ostep-projects/blob/master/initial-utilities/README.md)
repository. 




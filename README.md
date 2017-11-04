# Nips\_MT
This is the code for the paper "Practical Hash Functions for Similarity
Estimation and Dimensionality Reduction" at NIPS'17

The code is written in C++ and contains reference implementations of several
hash functions including Mixed Tabulation [1], Simple Tabulation [2], and
Twisted Tabulation [3] as well as implementation of all experiments in the
paper except the LSH experiments. The code also contains python scripts for
generating plots from the data.

## Overview
The code is split into several parts all contained in the src folder:
* src/framework contains the implementations of different hash functions and
  sketches.
* src/plotGenerator contains python scripts for generating plots once the
  experiments have been run. These require Numpy and Matplotlib to work.
* src/ contains the code for the experiments as well as a Makefile to compile
  them. It also contains the code to generate the synthetic datasets.

Before you try to run the code plese read the sections below on randomness and
data.

Also, for the code to work, you need to create a folder named "output" in the
src/ and the src/plotGenerator/ folders.

## Randomness
The code provided in this repo uses C++11 random generation as standard for
portability. *This is not intended!!* We strongly recommend that any user
downloads a large seed of random bytes from e.g. random.org and uses this
instead. There is information in src/framework/hashing.h,
src/framework/hashing\_more.h, and src/framework/sketches.h on how to use such
a random seed instead.

## Data
The news20 and MNIST data sets are not included in this repo. They can be
downloaded here:
[News20](https://www.csie.ntu.edu.tw/~cjlin/libsvmtools/datasets/binary.html)
and [MNIST](http://yann.lecun.com/exdb/mnist/)

To use the code, place the data files in the src/data/ folder and compile and
run the code of the news20\_change\_format.cpp file to create a data file with
the news20 dataset represented as indicator vectors.

## External software
The code uses the official implementations of MurmurHash3, CityHash, and
BLAKE2 and wraps these in a class similar to the reference implementations of
the other hash functions.

The official implementations of these hash functions are available at (and also
included in the src/framework folder):
* [MurmurHash3](https://github.com/aappleby/smhasher/wiki/MurmurHash3)
* [CityHash](https://github.com/google/cityhash)
* [BLAKE2](https://github.com/BLAKE2/BLAKE2)

## References
[1] Søren Dahlgaard, Mathias Bæk Tejs Knudsen, Eva Rotenberg, Mikkel Thorup: Hashing for Statistics over K-Partitions. FOCS 2015: 1292-1310

[2] Mihai Patrascu, Mikkel Thorup: The Power of Simple Tabulation Hashing. J. ACM 59(3): 14:1-14:50 (2012)

[3] Mihai Patrascu, Mikkel Thorup: Twisted Tabulation Hashing. SODA 2013: 209-228


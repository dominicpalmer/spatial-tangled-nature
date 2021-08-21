# Simulating tumour development in two and three dimensions using the framework of the spatial Tangled Nature model

[![Build Status](https://api.travis-ci.com/dominicpalmer/spatial-tangled-nature.svg?branch=main)](https://travis-ci.com/dominicpalmer/spatial-tangled-nature) ![License](https://img.shields.io/github/license/dominicpalmer/spatial-tangled-nature)

**Dominic Palmer - MSc Project - Imperial College London - 2018**

## Summary
My MSc project centred around the development of two and three-dimensional coarse-grained simulations of tumour growth based on a mathematical model called Tangled Nature. The simulations are lightweight genetic algorithms built on the notion of a lattice of nodes, each of which can sustain a population of cells with the ability to replicate, mutate, die, or relocate to a neighbouring node with some functional probability. This repository contains the source code of the 3D model.

Simulation constraints were tested to examine how varying the initial state and surrounding environment affects growth rates, heterogenity and response to treatment. Tangled Nature was created by Christensen et. al [[1]](#references), originally aimed at simulating evolutionary ecology. For further reading on the model, [[2]](#references) provides an overview.

The impact of resource allocation on growth in terms of population *N* and population density *D*:

<p align="center">
    <img align="center" src="maps.png" alt="drawing" width="600" />
</p>

## Prerequisites
Parameter constants specific to the Tangled Nature model can be defined in code prior to building from source, such as the probability of cell mutation, `P_MUT`. Some parameters aren't so self explanatory, so familiarity with the model underpinning the code may be required to make sensible choices.

## Dependencies
None for the simulation. GoogleTest for tests. The projects are GNU Make based and compile cross platform. On Linux I recommend using the native GNU toolchain, Clang if using OSX, and mingw-w64 or MSYS2 on Windows. Recipes to make use of LLVM tools clang-tidy and clang-format are baked into the makefiles, plus Valgrind for memory checks.

## Build and Run
Clone the repo:

```bash
git clone https://github.com/dominicpalmer/spatial-tangled-nature.git
```

Then clone GoogleTest into a directory named **lib**:

```bash
cd spatial-tangled-nature && mkdir lib
cd lib && git clone https://github.com/google/googletest.git -b release-1.10.0
```

To build the 3D simulation using g++ with -std=c++17, first define the parameter constants in **params.h** if you're wanting to diverge from the defaults. Then from the project root:

```bash
cd stn3d && make
bin/stn3d
```

Output is written to an **out** directory created at the invocation path at runtime. Clear the output by running *make clean*.

## Tests
From the project root:

```bash
cd stn3d && make tests
bin/stn3d_tests
```

## Output
The program will first write initial conditions and parameters to a file named **initial_state_log.txt** in the **out** directory. An additional log named **population_log.txt** is created and updated with the total population of the lattice at each generational step.

Log files per lattice point are also created and written to; they're named according to their lattice coordinates, e.g. **existent_genotypes_123.txt**. These logs track the 'genetic' diversity of the population for each lattice point. Written to them on a row by row basis at each generational step are all existent 'genotypes' local to the point, denoted in base 10.

## License
MIT licensed.

## References
[1] CHRISTENSEN, K., DI COLLOBIANO, S., HALL, M. and JENSEN, H. (2002). Tangled Nature: A Model of Evolutionary Ecology. Journal of Theoretical Biology, 216(1), pp.73-84.

[2] LAIRD, S., LAWSON, D. and JENSEN, H. (2008). The tangled nature model of evolutionary ecology: an overview. Mathematical Modelling of Biological Systems, 2(1), pp.49-62.
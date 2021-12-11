#ifndef PARAMS_H_
#define PARAMS_H_

#include <cinttypes>

// Ubiquitous constants relating to the spatial Tangled Nature model.
// The ambiguous macro names have been specifically chosen to mirror variable
// naming in the mathematical model, so brief descriptions are provided here.
// For a detailed explanation and guidance on usage, see:
// https://wwwf.imperial.ac.uk/~hjjens/Laird_Lawson_Jensen_4.pdf
constexpr uint16_t L = 12;                // The size of a genotypes bitset
constexpr uint16_t GENOTYPES_TOT = 4096;  // 2^L: The number of unique genotypes
constexpr uint16_t GENERATIONS_TOT = 500;  // Maximal generational steps
constexpr uint16_t X = 6;                  // Lattice dimension length
constexpr uint16_t N_0 = 100;              // Starting population size
constexpr double THETA = 0.25;   // Probability of nonzero interactions
constexpr double C_R = 20.0;     // Used in the reproduction weight function
constexpr double PMUT = 0.05;    // Probability of genotype mutation
constexpr double PKILL = 0.2;    // Probability of genotype death
constexpr double PMOVE = 0.003;  // Probability of genotype migration
constexpr bool FIX_MU = true;    // Flag to fix constant resources (mu)
constexpr double FIXED_MU_VAL = 0.05;  // Resource supply value when FIX_MU=true
constexpr bool CUBIC_MU = true;        // Flag for cubic distribution of mu
constexpr bool FIX_START = true;       // Flag for fixing start position
constexpr uint16_t FIXED_X_VAL = 3;    // x coordinate of starting position
constexpr uint16_t FIXED_Y_VAL = 3;    // y coordinate of starting position
constexpr uint16_t FIXED_Z_VAL = 3;    // z coordinate of starting position
constexpr bool RAND_OCC_SELECTION = false;  // Enforce random node selection

#endif
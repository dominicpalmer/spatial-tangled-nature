#ifndef UTIL_H_
#define UTIL_H_

#include <array>
#include <bitset>
#include <cinttypes>
#include <fstream>
#include <map>
#include <memory>
#include <vector>

#include "stn3d/params.h"

using LatticeCoord = uint8_t;
using LatticePoint = uint32_t;

struct Node {
  LatticeCoord i_coord;  // The nodes x coordinate in the lattice
  LatticeCoord j_coord;  // The nodes y coordinate in the lattice
  LatticeCoord k_coord;  // The nodes z coordinate in the lattice
  std::array<int, GENOTYPES_TOT> genotype_counts;  // Genotype population counts
  std::vector<int> existent_genotypes;   // Stores existent genotypes on node
  std::vector<LatticePoint> neighbours;  // Stores valid neighbouring nodes
  double mu;                             // Resource allocation on node
  int population;  // Node population: the sum of genotype_counts elements
};

extern std::array<std::array<std::array<std::unique_ptr<Node>, X>, X>, X> nodes;
extern std::map<
    LatticeCoord,
    std::map<LatticeCoord,
             std::map<LatticeCoord, std::unique_ptr<std::ofstream>>>>
    outfiles;
extern std::vector<LatticePoint> occupied_nodes;
extern std::array<double, GENOTYPES_TOT> arr_a1, arr_a2;
extern std::array<int, GENOTYPES_TOT> arr_b;
extern std::bitset<L> genotype_bitsets[GENOTYPES_TOT];

void ValidateParameters();
double UniformRealInRange(int min, int max);
int UniformIntInRange(int min, int max);
LatticePoint GetOccupiedNode();
LatticeCoord GetCoordinate(LatticePoint latticePoint, uint32_t idx);
void CloseAllOutputFiles();

#endif
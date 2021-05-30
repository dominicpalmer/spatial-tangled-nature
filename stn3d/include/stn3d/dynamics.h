#ifndef DYNAMICS_H_
#define DYNAMICS_H_

#include <cinttypes>
#include <map>
#include <vector>

#include "stn3d/params.h"

using LatticeCoord = uint8_t;
using LatticePoint = uint32_t;

double GetInteractionStrength(int genotype_a, int genotype_b);
int Reproduce(std::array<int, GENOTYPES_TOT> &g_counts,
              std::vector<int> &existent, int &N, double mu);
bool Annihilate(std::array<int, GENOTYPES_TOT> &g_counts,
                std::vector<int> &existent, int &N, int existent_idx,
                LatticeCoord i_coord, LatticeCoord j_coord,
                LatticeCoord k_coord);
void Migrate(std::vector<LatticePoint> &neighbours,
             std::array<int, GENOTYPES_TOT> &g_counts,
             std::vector<int> &existent, int &N, int existent_idx,
             LatticeCoord i_coord, LatticeCoord j_coord, LatticeCoord k_coord);
void SimLoop(LatticeCoord i_selection, LatticeCoord j_selection,
             LatticeCoord k_selection);

#endif
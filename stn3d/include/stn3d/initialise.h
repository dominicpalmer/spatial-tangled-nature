#ifndef INITIALISE_H_
#define INITIALISE_H_

#include <cinttypes>
#include <vector>

using LatticeCoord = uint8_t;
using LatticePoint = uint32_t;

void InitialiseGenotypes();
void InitialiseMatricies();
void InitialiseNeighbours(std::vector<LatticePoint> &neighbours,
                          LatticeCoord i_coord, LatticeCoord j_coord,
                          LatticeCoord k_coord);
void InitialiseLattice();
void InitialiseResources();
void InitialisePopulationOnNode(LatticeCoord i_coord, LatticeCoord j_coord,
                                LatticeCoord k_coord);
void LogInitialState(LatticeCoord i_coord, LatticeCoord j_coord,
                     LatticeCoord k_coord);
void DistributeCubicMu();
void DistributeGradientMu();

#endif
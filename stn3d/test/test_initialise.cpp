#include "gtest/gtest.h"
#include "stn3d/initialise.h"
#include "stn3d/util.h"

// Tests for successful initialisation of genotype bitsets
TEST(InitialiseGenotypes, BitsetsAreInitialised) {
  // Arrange: initialise all genotype bitsets
  InitialiseGenotypes();

  // Assert: the bitsets are initialised as expected
  int error = 0;
  for (int idx = 0; idx < GENOTYPES_TOT; idx++) {
    if (genotype_bitsets[idx] != std::bitset<L>(static_cast<uint64_t>(idx))) {
      error = 1;
    }
  }

  ASSERT_FALSE(error);
}

// Tests for successful initialisation of calculation matricies
TEST(InitialiseMatricies, MatriciesAreInitialised) {
  // Arrange: initialise the interaction calculation matricies
  InitialiseMatricies();

  // Assert:
  int error = 0;
  for (int idx = 0; idx < GENOTYPES_TOT; idx++) {
    if ((arr_a1[idx] < -1 || arr_a1[idx] >= 1) ||
        (arr_a2[idx] < -1 || arr_a2[idx] >= 1) ||
        (arr_b[idx] != 0 && arr_b[idx] != 1)) {
      error = 1;
    }
  }

  ASSERT_FALSE(error);
}

// Tests that InitialiseNeighbours correctly populates the neighbours vector of
// internal lattice points
TEST(InitialiseNeighbours, ForInternalLatticePoint_NeighboursPopulated) {
  // Arrange: initialise the neighbours vector of lattice point (1, 1, 1)
  InitialiseLattice();
  (*nodes[1][1][1]).neighbours.clear();
  InitialiseNeighbours((*nodes[1][1][1]).neighbours, 1, 1, 1);

  // Assert: the neighbours vector contains the expected lattice points
  auto* expected_neighbours = new std::vector<LatticePoint>{
      0,     65536,  131072, 256,   65792,  131328, 512,   66048,  131584,
      1,     65537,  131073, 257,   131329, 513,    66049, 131585, 2,
      65538, 131074, 258,    65794, 131330, 514,    66050, 131586};
  ASSERT_EQ(*expected_neighbours, (*nodes[1][1][1]).neighbours);

  delete expected_neighbours;
}

// Tests that InitialiseNeighbours correctly populates the neighbours vector of
// lattice points on a lattice boundary
TEST(InitialiseNeighbours, ForBoundaryLatticePoint_NeighboursPopulated) {
  // Arrange: initialise the neighbours vector of lattice point (0, 0, 0)
  InitialiseLattice();
  (*nodes[0][0][0]).neighbours.clear();
  InitialiseNeighbours((*nodes[0][0][0]).neighbours, 0, 0, 0);

  // Assert: the neighbours vector contains the expected lattice points
  auto* expected_neighbours = new std::vector<LatticePoint>{
      328965, 1285,  66821,  327685, 5,     65541,  327941, 261,   65797,
      328960, 1280,  66816,  327680, 65536, 327936, 256,    65792, 328961,
      1281,   66817, 327681, 1,      65537, 327937, 257,    65793};
  ASSERT_EQ(*expected_neighbours, (*nodes[0][0][0]).neighbours);

  delete expected_neighbours;
}

// Tests that population initialisation correctly pushes the target node to the
// occupied_nodes vector
TEST(InitialisePopulationOnNode, OccupiedNodesInitialised) {
  // Arrange: initialise the starting population on the node at (1, 1, 1)
  InitialiseLattice();
  occupied_nodes.clear();
  InitialisePopulationOnNode(1, 1, 1);

  // Assert: the occupied_nodes vector contains lattice point (1, 1, 1)
  std::vector<LatticePoint>::iterator it;
  it = find(occupied_nodes.begin(), occupied_nodes.end(), 65793);

  ASSERT_FALSE(it == occupied_nodes.end());
}

// Tests that population initialisation correctly assigns the starting
// population to the target node
TEST(InitialisePopulationOnNode, NodePopulationInitialised) {
  // Arrange: initialise the starting population on the node at (1, 1, 1)
  InitialiseLattice();
  (*nodes[1][1][1]).population = 0;
  InitialisePopulationOnNode(1, 1, 1);

  // Assert: the node population is strictly positive
  ASSERT_TRUE((*nodes[1][1][1]).population > 0);
}
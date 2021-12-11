#include "gtest/gtest.h"
#include "stn3d/dynamics.h"
#include "stn3d/initialise.h"
#include "stn3d/util.h"

void InitialiseTestLattice(int genotype, LatticePoint i, LatticePoint j,
                           LatticePoint k);

// Tests that the interaction strength between identical genotypes is zero
TEST(GetInteractionStrength, WhenSameGenotype_ZeroInteraction) {
  // Arrange: initialise interaction matricies
  InitialiseMatricies();

  // Act: calculate the interaction strength between two identical individuals
  double interaction_strength = GetInteractionStrength(1, 1);

  // Assert: the interaction strength is zero
  double expected_interaction_strength = 0.0;
  ASSERT_EQ(expected_interaction_strength, interaction_strength);
}

// Tests that the interaction between untangled genotypes is zero
TEST(GetInteractionStrength, WhenNoEntanglement_ZeroInteraction) {
  // Arrange: initialise interaction matricies with mock entanglement
  InitialiseMatricies();

  const int genotype_a = 1;
  const int genotype_b = 2;
  const int z = genotype_a ^ genotype_b;
  arr_b[z] = 0.0;

  // Act: calculate the interaction strength between untangled individuals
  double interaction_strength = GetInteractionStrength(genotype_a, genotype_b);

  // Assert: the interaction strength is zero
  double expected_interaction_strength = 0.0;
  ASSERT_EQ(expected_interaction_strength, interaction_strength);
}

// Tests that the interaction between different entangled genotypes is non-zero
TEST(GetInteractionStrength, WhenDifferentGenotype_NonzeroInteraction) {
  // Arrange: initialise interaction matricies with mock entanglement
  InitialiseMatricies();

  const int genotype_a = 1;
  const int genotype_b = 2;
  const int z = genotype_a ^ genotype_b;
  arr_b[z] = 1.0;

  // Act: calculate the interaction strength between entangled individuals
  double interaction_strength = GetInteractionStrength(genotype_a, genotype_b);

  // Assert: the interaction strength is non-zero
  double zero_interaction_strength = 0.0;
  ASSERT_NE(zero_interaction_strength, interaction_strength);
}

// Tests that reproduction returns a valid individual
TEST(Reproduce, ReturnsIndividual) {
  // Arrange: initialise a test lattice at (1, 1, 1)
  int genotype = 1234;
  LatticePoint i = 1;
  LatticePoint j = 1;
  LatticePoint k = 1;
  InitialiseTestLattice(genotype, i, j, k);

  // Act: make a call to Reproduce
  int individual = Reproduce(
      (*nodes[i][j][k]).genotype_counts, (*nodes[i][j][k]).existent_genotypes,
      (*nodes[i][j][k]).population, (*nodes[i][j][k]).mu);

  // Assert: a valid individual is returned
  ASSERT_TRUE(individual >= 0);
  ASSERT_TRUE(individual < GENOTYPES_TOT);
}

// Initialises a lattice with certainty of existence of a specific genotype at
// a specific node
void InitialiseTestLattice(const int genotype, const LatticePoint i,
                           const LatticePoint j, const LatticePoint k) {
  InitialiseGenotypes();
  InitialiseMatricies();
  InitialiseLattice();
  InitialiseResources();
  InitialisePopulationOnNode(i, j, k);

  (*nodes[i][j][k]).existent_genotypes.push_back(genotype);
  (*nodes[i][j][k]).genotype_counts[genotype]++;
  (*nodes[i][j][k]).population++;
}
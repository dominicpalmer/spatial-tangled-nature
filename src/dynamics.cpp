#include "stn3d/dynamics.h"

#include <algorithm>
#include <iostream>
#include <random>

#include "stn3d/util.h"

// Calculates J(a,b): the strength of the interaction between genotypes a and b
double GetInteractionStrength(const int genotype_a, const int genotype_b) {
  double jab = 0.0;

  // Jab = 0 for a = b
  if (genotype_a == genotype_b) {
    return jab;
  }

  // Interaction strengths are equal A1[z]A2[b] with probability theta
  const int z = genotype_a ^ genotype_b;
  if (arr_b[z]) {
    jab = arr_a1[z] * arr_a2[genotype_b];
  }

  return jab;
}

// Attempts reproduction of a randomly chosen individual, and returns that
// individual regardless of the result
int Reproduce(std::array<int, GENOTYPES_TOT> &g_counts,
              std::vector<int> &existent, int &N, const double mu) {
  // Randomly choose an individual (an existent genotype occupying the node)
  const int existent_size = static_cast<int>(existent.size());
  const int existent_idx = UniformIntInRange(0, existent_size - 1);
  const int individual = existent[existent_idx];

  // Calculate the sum component of H for the chosen individual
  double t1 = 0.0;
  double jab;
  for (int idx = 0; idx < existent_size; idx++) {
    jab = GetInteractionStrength(individual, existent[idx]);
    t1 += jab * g_counts[existent[idx]];
  }

  // Calculate the weight function (H) and poff
  const double weight_function = ((C_R * t1) / N) - (mu * N);
  const double poff = 1 / (1 + exp(-weight_function));

  // Try to reproduce the chosen individual
  int offspring = 0;
  std::bitset<L> genotype_bitstring = {0};
  if (UniformRealInRange(0, 1) <= poff) {
    N++;

    for (int idx = 0; idx < L; idx++) {
      genotype_bitstring[idx] = genotype_bitsets[individual][idx];

      // The offsprings 'genes' are mutated (bitflipped) with probability PMUT
      if (UniformRealInRange(0, 1) <= PMUT) {
        genotype_bitstring[idx] = ~genotype_bitstring[idx];
      }

      if (genotype_bitstring[idx]) {
        offspring += (1 << (L - 1 - idx));
      }
    }

    // Check for novel offspring
    if (g_counts[offspring] == 0) {
      existent.push_back(offspring);
    }

    g_counts[offspring]++;
  }

  return existent_idx;
}

// Attempts annihilation of a specified individual
bool Annihilate(std::array<int, GENOTYPES_TOT> &g_counts,
                std::vector<int> &existent, int &N, const int existent_idx,
                const LatticeCoord i, const LatticeCoord j,
                const LatticeCoord k) {
  if (UniformRealInRange(0, 1) <= PKILL) {
    N--;

    int individual = existent[existent_idx];
    g_counts[individual]--;

    if (g_counts[individual] == 0) {
      // Chosen genotype is extinct, so remove from the nodes existent vector
      existent.erase(existent.begin() + existent_idx);

      // If the node has become empty, remove it from the occupied_nodes vector
      if (N == 0) {
        const LatticePoint lattice_point =
            i + ((static_cast<char>(j)) << 8) + ((static_cast<char>(k)) << 16);
        auto vec_iter = std::find(occupied_nodes.begin(), occupied_nodes.end(),
                                  lattice_point);
        occupied_nodes.erase(vec_iter);
      }
    }

    return true;
  }

  return false;
}

// Attempts migration of a specified individual
void Migrate(std::vector<LatticePoint> &neighbours,
             std::array<int, GENOTYPES_TOT> &g_counts,
             std::vector<int> &existent, int &N, int existent_idx,
             const LatticeCoord i, const LatticeCoord j, const LatticeCoord k) {
  if (UniformRealInRange(0, 1) <= PMOVE) {
    N--;

    const int individual = existent[existent_idx];
    g_counts[individual]--;

    // Check if the migrated genotype is now extinct at the origin lattice point
    LatticePoint lattice_point;
    if (g_counts[individual] == 0) {
      existent.erase(existent.begin() + existent_idx);

      // Remove the genotype from occupied_nodes if the node population is zero
      if (N == 0) {
        lattice_point =
            i + ((static_cast<char>(j)) << 8) + ((static_cast<char>(k)) << 16);
        auto vec_iter =
            find(occupied_nodes.begin(), occupied_nodes.end(), lattice_point);
        occupied_nodes.erase(vec_iter);
      }
    }

    // Randomly choose a neighbouring lattice point of (i, j, k)
    const uint32_t neighbours_index =
        UniformIntInRange(0, static_cast<int>(neighbours.size()) - 1);
    lattice_point = neighbours[neighbours_index];
    LatticeCoord i_coord = GetCoordinate(lattice_point, 1);
    LatticeCoord j_coord = GetCoordinate(lattice_point, 2);
    LatticeCoord k_coord = GetCoordinate(lattice_point, 3);

    // If the destination lattice point is empty, add it to occupied_nodes
    if ((*nodes[i_coord][j_coord][k_coord]).population == 0) {
      occupied_nodes.push_back(lattice_point);
    }

    (*nodes[i_coord][j_coord][k_coord]).population++;

    // Add the migrating species to the existent_genotypes vector if the
    // desination node doesn't already contain it
    if ((*nodes[i_coord][j_coord][k_coord]).genotype_counts[individual] == 0) {
      (*nodes[i_coord][j_coord][k_coord])
          .existent_genotypes.push_back(individual);
    }

    // Increase the desination node species count of the migrated individual
    (*nodes[i_coord][j_coord][k_coord]).genotype_counts[individual]++;
  }
}

// Starts a new simulation loop using specified parameters
void SimLoop(LatticeCoord i_selection, LatticeCoord j_selection,
             LatticeCoord k_selection) {
  std::cout << "Lattice size: " << X << "x" << X << "\n"
            << "Generations: " << GENERATIONS_TOT << "\n"
            << "Starting population: " << N_0 << "\n"
            << "Starting coordinates: (" << static_cast<int>(i_selection)
            << ", " << static_cast<int>(j_selection) << ", "
            << static_cast<int>(k_selection) << ")\n"
            << "Generations completed:" << std::endl;

  // Write total population size by generation to a logfile
  std::ofstream population_log;
  population_log.open("out/population_log.txt");

  int gen_count = 0;
  int step = 0;
  int individual;
  int n_tot;
  LatticePoint lattice_point;
  bool annihilated;

  // Inital calculation of tau: the number of steps comprising one generation
  double tau =
      round(double((*nodes[i_selection][j_selection][k_selection]).population) /
            PKILL);

  while (gen_count < GENERATIONS_TOT) {
    step++;

    lattice_point = GetOccupiedNode();
    i_selection = GetCoordinate(lattice_point, 1);
    j_selection = GetCoordinate(lattice_point, 2);
    k_selection = GetCoordinate(lattice_point, 3);

    individual = Reproduce(
        (*nodes[i_selection][j_selection][k_selection]).genotype_counts,
        (*nodes[i_selection][j_selection][k_selection]).existent_genotypes,
        (*nodes[i_selection][j_selection][k_selection]).population,
        (*nodes[i_selection][j_selection][k_selection]).mu);

    annihilated = Annihilate(
        (*nodes[i_selection][j_selection][k_selection]).genotype_counts,
        (*nodes[i_selection][j_selection][k_selection]).existent_genotypes,
        (*nodes[i_selection][j_selection][k_selection]).population, individual,
        i_selection, j_selection, k_selection);

    if (!annihilated) {
      Migrate(
          (*nodes[i_selection][j_selection][k_selection]).neighbours,
          (*nodes[i_selection][j_selection][k_selection]).genotype_counts,
          (*nodes[i_selection][j_selection][k_selection]).existent_genotypes,
          (*nodes[i_selection][j_selection][k_selection]).population,
          individual, i_selection, j_selection, k_selection);
    }

    // Housekeeping at the end of each generation
    if (step == tau) {
      gen_count++;
      step = 0;

      // Log the existent species of each node
      n_tot = 0;
      for (int i = 0; i < X; i++) {
        for (int j = 0; j < X; j++) {
          for (int k = 0; k < X; k++) {
            for (int genotype : (*nodes[i][j][k]).existent_genotypes) {
              (*outfiles[i][j][k]) << genotype << "\t";
            }
            (*outfiles[i][j][k]) << std::endl;

            n_tot = n_tot + (*nodes[i][j][k]).population;
          }
        }
      }

      // Log population size against generation count
      population_log << gen_count << "\t" << n_tot << std::endl;

      // Recalculate tau
      tau = round(double(n_tot) / PKILL);

      if (gen_count % (std::max(GENERATIONS_TOT / 100, 1)) == 0) {
        std::cout << gen_count << std::endl;
      }
    }
  }

  std::cout << "All generations passed without extinction." << std::endl;
  population_log.close();
}
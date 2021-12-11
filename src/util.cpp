#include "stn3d/util.h"

#include <algorithm>
#include <bitset>
#include <iostream>
#include <random>
#include <sstream>

#include "stn3d/dynamics.h"

// Use of global data structures instead of a lattice state object is a design
// choice made to reduce pushes and pops to/from the stack, improving function
// call and indexing speed
std::array<std::array<std::array<std::unique_ptr<Node>, X>, X>, X> nodes;
std::map<LatticeCoord,
         std::map<LatticeCoord,
                  std::map<LatticeCoord, std::unique_ptr<std::ofstream>>>>
    outfiles;
std::vector<LatticePoint> occupied_nodes;
std::array<double, GENOTYPES_TOT> arr_a1;
std::array<double, GENOTYPES_TOT> arr_a2;
std::array<int, GENOTYPES_TOT> arr_b;
std::bitset<L> genotype_bitsets[GENOTYPES_TOT];
std::random_device random_device;
std::mt19937 twister_engine(random_device());

// Validates that user provided parameters conform to simulation limitations
void ValidateParameters() {
  std::stringstream oss;

  uint16_t validation_errors = 0;
  if (L <= 1 || L >= 17) {
    validation_errors += 1;
    oss << "L must be in [2, 16].\n";
  }
  if (GENOTYPES_TOT != pow(2, L)) {
    validation_errors += 1;
    oss << "GENOTYPES_TOT must be equal 2^L.\n";
  }
  if (X <= 1 || X >= 10) {
    validation_errors += 1;
    oss << "X must be in [2, 9].\n";
  }
  if (N_0 == 0) {
    validation_errors += 1;
    oss << "N_0 must be positive.\n";
  }
  if (THETA < 0) {
    validation_errors += 1;
    oss << "THETA must be non-negative.\n";
  }
  if (PMUT < 0) {
    validation_errors += 1;
    oss << "PMUT must be non-negative.\n";
  }
  if (PKILL < 0) {
    validation_errors += 1;
    oss << "PKILL must be non-negative.\n";
  }
  if (PMOVE < 0) {
    validation_errors += 1;
    oss << "PMOVE must be non-negative.\n";
  }
  if (FIX_START && (FIXED_X_VAL <= 1 || FIXED_X_VAL > X || FIXED_Y_VAL <= 1 ||
                    FIXED_Y_VAL > X || FIXED_Z_VAL <= 1 || FIXED_Z_VAL > X)) {
    validation_errors += 1;
    oss << "If using a fixed starting point, the starting coordinates "
           "FIXED_X_VAL, FIXED_Y_VAL and FIXED_Z_VAL must not exceed the "
           "lattice dimensions.\n";
  }
  if (FIX_MU && FIXED_MU_VAL < 0) {
    validation_errors += 1;
    oss << "FIXED_MU_VAL must be non-negative.\n";
  }

  if (validation_errors) {
    std::cout << "There are " << validation_errors
              << " parameter value errors associated with this build:\n"
              << std::endl;
    std::cout << oss.str() << std::endl;
    std::cout << "Please rebuild and run with well-defined parameters."
              << std::endl;

    exit(EXIT_FAILURE);
  }
}

// Returns a random floating-point number uniformly distributed over [min, max)
double UniformRealInRange(const int min, const int max) {
  std::uniform_real_distribution<> dist(min, max);
  return dist(twister_engine);
}

// Returns a random integer uniformly distributed over [min, max]
int UniformIntInRange(const int min, const int max) {
  std::uniform_int_distribution<> dist(min, max);
  return dist(twister_engine);
}

// Returns a lattice point home to an occupied node, according to the nodes
// population and relative time spent occupied
LatticePoint GetOccupiedNode() {
  if (occupied_nodes.empty()) {
    std::cout << "Total extinction." << std::endl;
    CloseAllOutputFiles();
    exit(EXIT_SUCCESS);
  } else {
    if (RAND_OCC_SELECTION) {
      return occupied_nodes[UniformIntInRange(
          0, static_cast<int>(occupied_nodes.size()) - 1)];
    }

    // Calculate total population
    int n_tot = 0;
    for (int i = 0; i < X; i++) {
      for (int j = 0; j < X; j++) {
        for (int k = 0; k < X; k++) {
          n_tot += (*nodes[i][j][k]).population;
        }
      }
    }

    // Node selection favours those with large populations relative to the
    // total, and those occupied the longest
    double running_population_perc = 0.0;
    const double threshold = UniformRealInRange(0, 1);
    for (LatticePoint lattice_point : occupied_nodes) {
      const LatticeCoord i_coord = GetCoordinate(lattice_point, 1);
      const LatticeCoord j_coord = GetCoordinate(lattice_point, 2);
      const LatticeCoord k_coord = GetCoordinate(lattice_point, 3);

      // Calculate node population as percentage of total
      const float node_weight =
          float((*nodes[i_coord][j_coord][k_coord]).population) / float(n_tot);

      // If the probability threshold is crossed, return the point
      running_population_perc += node_weight;
      if (running_population_perc >= threshold) {
        return lattice_point;
      }
    }
  }

  return 0;
}

// Returns the specified coordinate of a lattice point
LatticeCoord GetCoordinate(LatticePoint latticePoint, const uint32_t idx) {
  switch (idx) {
    case 1:
      latticePoint &= 0xff;  // First byte
      break;
    case 2:
      latticePoint &= 0xff00;
      latticePoint = latticePoint >> 8;  // Second byte
      break;
    case 3:
      latticePoint &= 0xff0000;
      latticePoint = latticePoint >> 16;  // Third byte
      break;
    default:
      break;
  }

  return latticePoint;
}

// Closes the existent species output file for each node
void CloseAllOutputFiles() {
  for (int i = 0; i < X; i++) {
    for (int j = 0; j < X; j++) {
      for (int k = 0; k < X; k++) {
        outfiles[i][j][k]->close();
      }
    }
  }
}
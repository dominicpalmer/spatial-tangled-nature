#include "stn3d/initialise.h"

#include <random>
#include <sstream>

#include "stn3d/util.h"

// Initialises the binary_values bitset array with genotype values
void InitialiseGenotypes() {
  for (int idx = 0; idx < GENOTYPES_TOT; idx++) {
    genotype_bitsets[idx] = std::bitset<L>(static_cast<uint64_t>(idx));
  }
}

// Initialises arrays A1, A2 and B for use in interaction calculations
void InitialiseMatricies() {
  for (int idx = 0; idx < GENOTYPES_TOT; idx++) {
    arr_a1[idx] = UniformRealInRange(-1, 1);
    arr_a2[idx] = UniformRealInRange(-1, 1);
    arr_b[idx] = UniformRealInRange(0, 1) <= THETA ? 1 : 0;
  }
}

// Fills a neighbours vector with its neighbouring lattice points
void InitialiseNeighbours(std::vector<LatticePoint> &neighbours,
                          const LatticeCoord i_coord,
                          const LatticeCoord j_coord,
                          const LatticeCoord k_coord) {
  int i_selection;
  int j_selection;
  int k_selection;
  LatticePoint lattice_point;

  // Cover all unit distances from point (i, j, k)
  for (int i_delta = -1; i_delta < 2; i_delta++) {
    for (int j_delta = -1; j_delta < 2; j_delta++) {
      for (int k_delta = -1; k_delta < 2; k_delta++) {
        // Compute potential neighbour coordinates
        i_selection = i_coord + i_delta;
        j_selection = j_coord + j_delta;
        k_selection = k_coord + k_delta;

        if (abs(i_delta) + abs(j_delta) + abs(k_delta) != 0) {
          if (i_selection <= X - 1 && j_selection <= X - 1 &&
              k_selection <= X - 1 && i_selection >= 0 && j_selection >= 0 &&
              k_selection >= 0) {
            // Store coordinates contained within the lattice boundaries
            lattice_point = i_selection +
                            ((static_cast<char>(j_selection)) << 8) +
                            ((static_cast<char>(k_selection)) << 16);
            neighbours.push_back(lattice_point);
          } else {
            // Else use periodic boundary conditions
            if (i_selection == X) {
              i_selection = 0;
            }
            if (j_selection == X) {
              j_selection = 0;
            }
            if (k_selection == X) {
              k_selection = 0;
            }
            if (i_selection == -1) {
              i_selection = X - 1;
            }
            if (j_selection == -1) {
              j_selection = X - 1;
            }
            if (k_selection == -1) {
              k_selection = X - 1;
            }

            lattice_point = i_selection +
                            ((static_cast<char>(j_selection)) << 8) +
                            ((static_cast<char>(k_selection)) << 16);
            neighbours.push_back(lattice_point);
          }
        }
      }
    }
  }
}

// Populates the lattice of nodes and creates output files ready for logging
void InitialiseLattice() {
  for (int i = 0; i < X; i++) {
    for (int j = 0; j < X; j++) {
      for (int k = 0; k < X; k++) {
        nodes[i][j][k] = std::make_unique<Node>();
        (*nodes[i][j][k]).i_coord = i;
        (*nodes[i][j][k]).j_coord = j;
        (*nodes[i][j][k]).k_coord = k;
        (*nodes[i][j][k]).population = 0;

        InitialiseNeighbours((*nodes[i][j][k]).neighbours, i, j, k);

        // Create and open an output file to track existent genotypes of node_ij
        std::ostringstream oss;
        oss << "out/existent_genotypes_" << i << j << k << ".txt";
        outfiles[i][j][k] = std::make_unique<std::ofstream>(oss.str());
      }
    }
  }
}

// Initialises lattice resources through distribution of mu
void InitialiseResources() {
  if (FIX_MU) {
    for (int k = 0; k < X; k++) {
      for (int j = 0; j < X; j++) {
        for (int i = 0; i < X; i++) {
          (*nodes[i][j][k]).mu = FIXED_MU_VAL;
        }
      }
    }
  } else {
    if (CUBIC_MU) {
      DistributeCubicMu();
    } else {
      DistributeGradientMu();
    }
  }
}

// Initialises the starting population N_0 on the specified node
void InitialisePopulationOnNode(const LatticeCoord i_coord,
                                const LatticeCoord j_coord,
                                const LatticeCoord k_coord) {
  LatticePoint lattice_point = i_coord + ((static_cast<char>(j_coord)) << 8) +
                               ((static_cast<char>(k_coord)) << 16);
  occupied_nodes.push_back(lattice_point);

  (*nodes[i_coord][j_coord][k_coord]).population = N_0;

  // Populate lattice point with N_0 randomly or explicitly chosen individuals
  int individual;
  for (int idx = 0; idx < N_0; idx++) {
    individual = UniformIntInRange(0, GENOTYPES_TOT - 1);

    // If the chosen individual is not yet occupying the node, add its label
    // to the existent_genotypes vector
    if ((*nodes[i_coord][j_coord][k_coord]).genotype_counts[individual] == 0) {
      (*nodes[i_coord][j_coord][k_coord])
          .existent_genotypes.push_back(individual);
    }

    // Increment the nodes occupancy of the chosen individual
    (*nodes[i_coord][j_coord][k_coord]).genotype_counts[individual] += 1;
  }
}

// Writes parameters and starting conditions to a logfile
void LogInitialState(const LatticeCoord i_coord, const LatticeCoord j_coord,
                     const LatticeCoord k_coord) {
  std::ofstream initial_state_log;
  initial_state_log.open("out/initial_state_log.txt");

  // Parameters
  initial_state_log << 'g' << GENERATIONS_TOT << '\t' << 'L' << L << '\t' << 'X'
                    << X << '\t' << 'p' << N_0 << '\t' << 't' << THETA << '\t'
                    << '\n';
  initial_state_log << 'm' << PMUT << '\t' << 'k' << PKILL << '\t' << 'v'
                    << PMOVE << '\t' << 'c' << C_R << "\n\n";

  // Mu
  initial_state_log << 'x' << '\t' << 'y' << '\t' << 'z' << '\t' << '\n';
  for (int k = 0; k < X; k++) {
    for (int j = 0; j < X; j++) {
      for (int i = 0; i < X; i++) {
        initial_state_log << i << '\t' << j << '\t' << k << '\t'
                          << (*nodes[i][j][k]).mu << '\n';
      }
      initial_state_log << '\n';
    }
    initial_state_log << '\n';
  }

  // Starting coordinates
  initial_state_log << "Staring coordinates: (" << (int)i_coord << ", "
                    << (int)j_coord << ", " << (int)k_coord << ")" << std::endl;

  initial_state_log.close();
}

// Distributes resources (mu) in a square frame pattern according to depth
void DistributeCubicMu() {
  int i;
  int j;
  int k;
  int frame_length;

  // Sum over the possible cube frame lengths
  for (frame_length = X; frame_length > 0; frame_length -= 2) {
    // Calculate the start and limit coordinates for the given frame length
    int frame_min = (X - frame_length) / 2;
    int frame_lim = (X + frame_length) / 2;

    for (i = frame_min; i < frame_lim; i++) {
      // If at a corner in the i-axis, also loop over j and k to include the
      // whole face
      if (i == frame_min || i == frame_lim - 1) {
        for (j = frame_min; j < frame_lim; j++) {
          for (k = frame_min; k < frame_lim; k++) {
            // Initialise mu according to the frame length
            (*nodes[i][j][k]).mu = (frame_length / (double(10) * X)) +
                                   (UniformRealInRange(-1, 1) / double(200));
            (*nodes[i][j][k]).mu -= fmod((*nodes[i][j][k]).mu, 0.001);
          }
        }
      }
      // Else if in the middle of a frames side on the i-axis, only loop over
      // the adjacent components of the frame
      else {
        for (j = frame_min; j < frame_lim; j++) {
          // If at a corner in the j-axis, also loop vertically
          if (j == frame_min || j == frame_lim - 1) {
            for (k = frame_min; k < frame_lim; k++) {
              (*nodes[i][j][k]).mu = (frame_length / (double(10) * X)) +
                                     (UniformRealInRange(-1, 1) / double(200));
              (*nodes[i][j][k]).mu -= fmod((*nodes[i][j][k]).mu, 0.001);
            }
          }
          // Else if in the middle of a frames side on the j-axis, only loop
          // over the base and vertical limit components
          else {
            for (k = frame_min; k < frame_lim; k += (frame_length - 1)) {
              (*nodes[i][j][k]).mu = (frame_length / (double(10) * X)) +
                                     (UniformRealInRange(-1, 1) / double(200));
              (*nodes[i][j][k]).mu -= fmod((*nodes[i][j][k]).mu, 0.001);
            }
          }
        }
      }
    }
  }
}

// Distributes resources (mu) in a gradient pattern, proportional to the x-axis
void DistributeGradientMu() {
  for (int i = 0; i < X; i++) {
    for (int j = 0; j < X; j++) {
      for (int k = 0; k < X; k++) {
        do {
          (*nodes[i][j][k]).mu = 0.1 - (i / (double(10) * X)) +
                                 (UniformRealInRange(-1, 1) / double(100));
          (*nodes[i][j][k]).mu -= fmod((*nodes[i][j][k]).mu, 0.001);
        } while ((*nodes[i][j][k]).mu <= 0.0);
      }
    }
  }
}
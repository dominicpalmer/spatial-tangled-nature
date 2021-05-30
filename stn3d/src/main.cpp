/*
----------------------- 3D Spatial Tangled Nature Model -----------------------
------------------------------- Dominic Palmer --------------------------------
-------------------------- Imperial College London ----------------------------
-------------------------------- March 2018 -----------------------------------

This program runs a three-dimensional spatial Tangled Nature Model on a cubic
lattice of X^3 nodes. Configure parameters to be used in params.h, then build
using the makefile.

-------------------------------------------------------------------------------
*/

#include <sys/stat.h>

#include "stn3d/dynamics.h"
#include "stn3d/initialise.h"
#include "stn3d/util.h"

// For use of _mkdir on Windows
#if defined(_WIN32) || defined(_WIN64)
#include <direct.h>
#endif

// Main entry
int main() {
  ValidateParameters();

  // Create an out directory if it doesn't already exist
  struct stat info;
#if defined(_WIN32) || defined(_WIN64)
  _setmaxstdio(1024);
  if (stat("out", &info)) {
    _mkdir("./out");
  }
#else
  if (stat("out", &info)) {
    system("mkdir out");
  }
#endif

  InitialiseGenotypes();
  InitialiseMatricies();
  InitialiseLattice();
  InitialiseResources();

  LatticeCoord i_start = FIX_START ? FIXED_X_VAL : UniformIntInRange(0, X - 1);
  LatticeCoord j_start = FIX_START ? FIXED_Y_VAL : UniformIntInRange(0, X - 1);
  LatticeCoord k_start = FIX_START ? FIXED_Z_VAL : UniformIntInRange(0, X - 1);
  InitialisePopulationOnNode(i_start, j_start, k_start);
  LogInitialState(i_start, j_start, k_start);

  SimLoop(i_start, j_start, k_start);

  CloseAllOutputFiles();

  return EXIT_SUCCESS;
}
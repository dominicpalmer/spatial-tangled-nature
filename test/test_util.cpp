#include "gtest/gtest.h"
#include "stn3d/initialise.h"
#include "stn3d/util.h"

// Tests that attempted retrieval of an occupied node on an empty lattice
// results in graceful exit
TEST(GetOccupiedNode, WhenNoOccupiedNodes_ExitSuccess) {
  // Arrange: initialise a lattice but force occupied_nodes to be empty
  InitialiseLattice();
  occupied_nodes.clear();

  // Assert: attempting to get an occupied node forces graceful exit
  ASSERT_EXIT(GetOccupiedNode(), ::testing::ExitedWithCode(0), "");
}

// Tests that attempted retrieval of an occupied node on a populated lattice
// results in a lattice point being returned
TEST(GetOccupiedNode, WhenOccupiedNode_ReturnsValue) {
  // Arrange: initialise a lattice with guaranteed occupancy of a node
  InitialiseLattice();
  InitialisePopulationOnNode(1, 1, 1);

  // Act: attempt to get an occupied node
  LatticePoint lattice_point = GetOccupiedNode();

  // Assert: the returned lattice point is as expected
  ASSERT_EQ(65793, lattice_point);
}

// Tests that GetCoordinate correctly returns the first coordinate
TEST(GetCoordinate, WhenFirstCoordRequested_FirstCoordReturned) {
  // Arrange: create a lattice point representing coordinates (1, 2, 3)
  LatticePoint lattice_point = 197121;

  // Act: get the first coordinate
  LatticeCoord i = GetCoordinate(lattice_point, 1);

  // Assert: the first coordinate is returned
  ASSERT_EQ(1, i);
}

// Tests that GetCoordinate correctly returns the second coordinate
TEST(GetCoordinate, WhenSecondCoordRequested_SecondCoordReturned) {
  // Arrange: create a lattice point representing coordinates (1, 2, 3)
  LatticePoint lattice_point = 197121;

  // Act: get the second coordinate
  LatticeCoord j = GetCoordinate(lattice_point, 2);

  // Assert: the second coordinate is returned
  ASSERT_EQ(2, j);
}

// Tests that GetCoordinate correctly returns the third coordinate
TEST(GetCoordinate, WhenThirdCoordRequested_ThirdCoordReturned) {
  // Arrange: create a lattice point representing coordinates (1, 2, 3)
  LatticePoint lattice_point = 197121;

  // Act: get the second coordinate
  LatticeCoord k = GetCoordinate(lattice_point, 3);

  // Assert: the second coordinate is returned
  ASSERT_EQ(3, k);
}
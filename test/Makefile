# A makefile for building stn3d_tests - the three-dimensional Spatial Tangled
# Nature simulation tests. Call from the project root makefile via:
# $ make tests
#
# Targets:
# make: build the stn3d_tests executable using g++ with -std=c++17

CPPFLAGS += -isystem $(GTEST_DIR)/include
CXXFLAGS += -g -O0 -std=c++17 -pedantic -Wall -Wextra -pthread

OBJ_DIR = ../obj
BIN_DIR = ../bin

# stn3d code
STN3D_INC_DIR = ../include
STN3D_INC = $(STN3D_INC_DIR)/stn3d/*.h
STN3D_OBJ = ../obj/*.o

# stn3d test code
STN3D_TEST_OBJ = $(OBJ_DIR)/test_util.o $(OBJ_DIR)/test_dynamics.o \
$(OBJ_DIR)/test_initialise.o

# GoogleTest code
GTEST_DIR = ../lib/googletest/googletest
GTEST_INC = $(GTEST_DIR)/include/gtest/*.h \
			$(GTEST_DIR)/include/gtest/internal/*.h
GTEST_SRC = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_INC)

# Built libraries
BUILT_LIBS = $(OBJ_DIR)/gtest_main.a $(OBJ_DIR)/stn3d.a

.PHONY: stn3d_tests

all: stn3d_tests

# Build static GoogleTest libraries gtest.a and gtest_main.a
$(OBJ_DIR)/gtest-all.o: $(GTEST_SRC) | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
	$(GTEST_DIR)/src/gtest-all.cc -o $@

$(OBJ_DIR)/gtest_main.o: $(GTEST_SRC) | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
	$(GTEST_DIR)/src/gtest_main.cc -o $@

$(OBJ_DIR)/gtest.a: $(OBJ_DIR)/gtest-all.o | $(OBJ_DIR)
	$(AR) $(ARFLAGS) $@ $^

$(OBJ_DIR)/gtest_main.a: $(OBJ_DIR)/gtest-all.o $(OBJ_DIR)/gtest_main.o \
| $(OBJ_DIR)
	$(AR) $(ARFLAGS) $@ $^

# Build static library stn3d.a from stn3d objects
$(OBJ_DIR)/stn3d.a: $(STN3D_OBJ)
	$(AR) $(ARFLAGS) $@ $^

# Build all tests
$(OBJ_DIR)/test_dynamics.o: test_dynamics.cpp $(GTEST_INC) $(STN3D_INC) \
| $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) -I$(STN3D_INC_DIR) $(CXXFLAGS) -c test_dynamics.cpp \
	-o $@

$(OBJ_DIR)/test_util.o: test_util.cpp $(GTEST_INC) $(STN3D_INC) | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) -I$(STN3D_INC_DIR) $(CXXFLAGS) -c test_util.cpp \
	-o $@

$(OBJ_DIR)/test_initialise.o: test_initialise.cpp $(GTEST_INC) $(STN3D_INC)  \
| $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) -I$(STN3D_INC_DIR) $(CXXFLAGS) -c test_initialise.cpp \
	-o $@

# Link the test executable
stn3d_tests: $(STN3D_TEST_OBJ) $(BUILT_LIBS)
	$(CXX) $(CXXFLAGS) -lpthread $^ -o $(BIN_DIR)/stn3d_tests
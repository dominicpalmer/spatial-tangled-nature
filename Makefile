# A makefile for building stn3d - the three-dimensional spatial Tangled
# Nature simulation.
#
# Targets:
# make: build the stn3d executable using g++ with -std=c++17
# make tests: build the stn3d_tests executable using g++ with -std=c++17
# make reset: delete all output files from ./out
# make clean: delete built executables from ./bin and object files from ./obj
# make format: format the source using Google's C++ coding standards
# make tidy: run make-tidy against the source
# make valgrind: run valgrind against the executable

CXX = g++
CXXFLAGS += -g -Wall -Wextra -pedantic -pthread -std=c++17
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
OBJECTS = $(OBJ_DIR)/main.o $(OBJ_DIR)/dynamics.o $(OBJ_DIR)/util.o \
		  $(OBJ_DIR)/initialise.o

CXXFLAGS += -Iinclude/

# Build system switch
ifeq ($(shell echo "windows"), "windows")
	EXE = .\bin\stn3d.exe .\bin\stn3d_tests.exe
	RM = del
	CLEAN_OBJS = $(OBJ_DIR)\*.o
	CLEAN_LIBS = $(OBJ_DIR)\*.a
	CLEAN_OUT = out\*.txt
else
	EXE = bin/stn3d bin/stn3d_tests
	RM = rm -f
	CLEAN_OBJS = $(OBJ_DIR)/*.o
	CLEAN_LIBS = $(OBJ_DIR)/*.a
	CLEAN_OUT = out/*.txt
endif

.PHONY: stn3d

# Link to stn3d
$(BIN_DIR)/stn3d: $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(BIN_DIR)/stn3d

# Build main object
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/dynamics.o: $(SRC_DIR)/dynamics.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/util.o: $(SRC_DIR)/util.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/initialise.o: $(SRC_DIR)/initialise.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Creates obj and bin directories if they don't already exist
obj:
	mkdir $@

bin:
	mkdir $@

tests:
	make
	make -C ./test

reset:
	$(RM) $(CLEAN_OUT)

clean:
	$(RM) $(CLEAN_OBJS)
	$(RM) $(CLEAN_LIBS)
	$(RM) $(EXE)

format:
	clang-format-10 -i src/*.cpp src/*.h

tidy:
	clang-tidy-10 src/* -header-filter=.* -- -xc++ -std=c++17 -Iinclude/

valgrind:
	valgrind $(BIN_DIR)/stn3d
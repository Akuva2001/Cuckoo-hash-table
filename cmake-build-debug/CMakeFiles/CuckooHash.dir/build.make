# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /snap/clion/149/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/149/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ivan/CLionProjects/CuckooHash

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ivan/CLionProjects/CuckooHash/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/CuckooHash.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/CuckooHash.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/CuckooHash.dir/flags.make

CMakeFiles/CuckooHash.dir/main.cpp.o: CMakeFiles/CuckooHash.dir/flags.make
CMakeFiles/CuckooHash.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ivan/CLionProjects/CuckooHash/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/CuckooHash.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/CuckooHash.dir/main.cpp.o -c /home/ivan/CLionProjects/CuckooHash/main.cpp

CMakeFiles/CuckooHash.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CuckooHash.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ivan/CLionProjects/CuckooHash/main.cpp > CMakeFiles/CuckooHash.dir/main.cpp.i

CMakeFiles/CuckooHash.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CuckooHash.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ivan/CLionProjects/CuckooHash/main.cpp -o CMakeFiles/CuckooHash.dir/main.cpp.s

# Object files for target CuckooHash
CuckooHash_OBJECTS = \
"CMakeFiles/CuckooHash.dir/main.cpp.o"

# External object files for target CuckooHash
CuckooHash_EXTERNAL_OBJECTS =

CuckooHash: CMakeFiles/CuckooHash.dir/main.cpp.o
CuckooHash: CMakeFiles/CuckooHash.dir/build.make
CuckooHash: CMakeFiles/CuckooHash.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ivan/CLionProjects/CuckooHash/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable CuckooHash"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/CuckooHash.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/CuckooHash.dir/build: CuckooHash

.PHONY : CMakeFiles/CuckooHash.dir/build

CMakeFiles/CuckooHash.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/CuckooHash.dir/cmake_clean.cmake
.PHONY : CMakeFiles/CuckooHash.dir/clean

CMakeFiles/CuckooHash.dir/depend:
	cd /home/ivan/CLionProjects/CuckooHash/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ivan/CLionProjects/CuckooHash /home/ivan/CLionProjects/CuckooHash /home/ivan/CLionProjects/CuckooHash/cmake-build-debug /home/ivan/CLionProjects/CuckooHash/cmake-build-debug /home/ivan/CLionProjects/CuckooHash/cmake-build-debug/CMakeFiles/CuckooHash.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/CuckooHash.dir/depend

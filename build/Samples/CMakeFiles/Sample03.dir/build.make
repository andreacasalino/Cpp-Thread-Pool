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
CMAKE_COMMAND = /snap/cmake/743/bin/cmake

# The command to remove a file.
RM = /snap/cmake/743/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/andrea/Scrivania/Thread-Pool

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/andrea/Scrivania/Thread-Pool/build

# Include any dependencies generated for this target.
include Samples/CMakeFiles/Sample03.dir/depend.make

# Include the progress variables for this target.
include Samples/CMakeFiles/Sample03.dir/progress.make

# Include the compile flags for this target's objects.
include Samples/CMakeFiles/Sample03.dir/flags.make

Samples/CMakeFiles/Sample03.dir/Main03.cpp.o: Samples/CMakeFiles/Sample03.dir/flags.make
Samples/CMakeFiles/Sample03.dir/Main03.cpp.o: ../Samples/Main03.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/andrea/Scrivania/Thread-Pool/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object Samples/CMakeFiles/Sample03.dir/Main03.cpp.o"
	cd /home/andrea/Scrivania/Thread-Pool/build/Samples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Sample03.dir/Main03.cpp.o -c /home/andrea/Scrivania/Thread-Pool/Samples/Main03.cpp

Samples/CMakeFiles/Sample03.dir/Main03.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Sample03.dir/Main03.cpp.i"
	cd /home/andrea/Scrivania/Thread-Pool/build/Samples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/andrea/Scrivania/Thread-Pool/Samples/Main03.cpp > CMakeFiles/Sample03.dir/Main03.cpp.i

Samples/CMakeFiles/Sample03.dir/Main03.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Sample03.dir/Main03.cpp.s"
	cd /home/andrea/Scrivania/Thread-Pool/build/Samples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/andrea/Scrivania/Thread-Pool/Samples/Main03.cpp -o CMakeFiles/Sample03.dir/Main03.cpp.s

# Object files for target Sample03
Sample03_OBJECTS = \
"CMakeFiles/Sample03.dir/Main03.cpp.o"

# External object files for target Sample03
Sample03_EXTERNAL_OBJECTS =

Samples/Sample03: Samples/CMakeFiles/Sample03.dir/Main03.cpp.o
Samples/Sample03: Samples/CMakeFiles/Sample03.dir/build.make
Samples/Sample03: Lib/libThread-Pool.a
Samples/Sample03: Samples/CMakeFiles/Sample03.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/andrea/Scrivania/Thread-Pool/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Sample03"
	cd /home/andrea/Scrivania/Thread-Pool/build/Samples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Sample03.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Samples/CMakeFiles/Sample03.dir/build: Samples/Sample03

.PHONY : Samples/CMakeFiles/Sample03.dir/build

Samples/CMakeFiles/Sample03.dir/clean:
	cd /home/andrea/Scrivania/Thread-Pool/build/Samples && $(CMAKE_COMMAND) -P CMakeFiles/Sample03.dir/cmake_clean.cmake
.PHONY : Samples/CMakeFiles/Sample03.dir/clean

Samples/CMakeFiles/Sample03.dir/depend:
	cd /home/andrea/Scrivania/Thread-Pool/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/andrea/Scrivania/Thread-Pool /home/andrea/Scrivania/Thread-Pool/Samples /home/andrea/Scrivania/Thread-Pool/build /home/andrea/Scrivania/Thread-Pool/build/Samples /home/andrea/Scrivania/Thread-Pool/build/Samples/CMakeFiles/Sample03.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Samples/CMakeFiles/Sample03.dir/depend


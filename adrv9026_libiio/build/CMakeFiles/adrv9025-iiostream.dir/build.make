# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.24

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/root/adrv9026_libiio

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/root/adrv9026_libiio/build

# Include any dependencies generated for this target.
include CMakeFiles/adrv9025-iiostream.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/adrv9025-iiostream.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/adrv9025-iiostream.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/adrv9025-iiostream.dir/flags.make

CMakeFiles/adrv9025-iiostream.dir/adrv9025-iiostream.cpp.o: CMakeFiles/adrv9025-iiostream.dir/flags.make
CMakeFiles/adrv9025-iiostream.dir/adrv9025-iiostream.cpp.o: /home/root/adrv9026_libiio/adrv9025-iiostream.cpp
CMakeFiles/adrv9025-iiostream.dir/adrv9025-iiostream.cpp.o: CMakeFiles/adrv9025-iiostream.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/root/adrv9026_libiio/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/adrv9025-iiostream.dir/adrv9025-iiostream.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/adrv9025-iiostream.dir/adrv9025-iiostream.cpp.o -MF CMakeFiles/adrv9025-iiostream.dir/adrv9025-iiostream.cpp.o.d -o CMakeFiles/adrv9025-iiostream.dir/adrv9025-iiostream.cpp.o -c /home/root/adrv9026_libiio/adrv9025-iiostream.cpp

CMakeFiles/adrv9025-iiostream.dir/adrv9025-iiostream.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/adrv9025-iiostream.dir/adrv9025-iiostream.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/root/adrv9026_libiio/adrv9025-iiostream.cpp > CMakeFiles/adrv9025-iiostream.dir/adrv9025-iiostream.cpp.i

CMakeFiles/adrv9025-iiostream.dir/adrv9025-iiostream.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/adrv9025-iiostream.dir/adrv9025-iiostream.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/root/adrv9026_libiio/adrv9025-iiostream.cpp -o CMakeFiles/adrv9025-iiostream.dir/adrv9025-iiostream.cpp.s

# Object files for target adrv9025-iiostream
adrv9025__iiostream_OBJECTS = \
"CMakeFiles/adrv9025-iiostream.dir/adrv9025-iiostream.cpp.o"

# External object files for target adrv9025-iiostream
adrv9025__iiostream_EXTERNAL_OBJECTS =

adrv9025-iiostream: CMakeFiles/adrv9025-iiostream.dir/adrv9025-iiostream.cpp.o
adrv9025-iiostream: CMakeFiles/adrv9025-iiostream.dir/build.make
adrv9025-iiostream: libiio-helper.a
adrv9025-iiostream: CMakeFiles/adrv9025-iiostream.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/root/adrv9026_libiio/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable adrv9025-iiostream"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/adrv9025-iiostream.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/adrv9025-iiostream.dir/build: adrv9025-iiostream
.PHONY : CMakeFiles/adrv9025-iiostream.dir/build

CMakeFiles/adrv9025-iiostream.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/adrv9025-iiostream.dir/cmake_clean.cmake
.PHONY : CMakeFiles/adrv9025-iiostream.dir/clean

CMakeFiles/adrv9025-iiostream.dir/depend:
	cd /home/root/adrv9026_libiio/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/root/adrv9026_libiio /home/root/adrv9026_libiio /home/root/adrv9026_libiio/build /home/root/adrv9026_libiio/build /home/root/adrv9026_libiio/build/CMakeFiles/adrv9025-iiostream.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/adrv9025-iiostream.dir/depend


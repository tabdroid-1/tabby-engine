# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.27

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
CMAKE_SOURCE_DIR = /home/tabby/Projects/C++/TabbyRay

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/tabby/Projects/C++/TabbyRay/build

# Utility rule file for ContinuousMemCheck.

# Include any custom commands dependencies for this target.
include Tabby/vendor/raylib-tmx/CMakeFiles/ContinuousMemCheck.dir/compiler_depend.make

# Include the progress variables for this target.
include Tabby/vendor/raylib-tmx/CMakeFiles/ContinuousMemCheck.dir/progress.make

Tabby/vendor/raylib-tmx/CMakeFiles/ContinuousMemCheck:
	cd /home/tabby/Projects/C++/TabbyRay/build/Tabby/vendor/raylib-tmx && /usr/bin/ctest -D ContinuousMemCheck

ContinuousMemCheck: Tabby/vendor/raylib-tmx/CMakeFiles/ContinuousMemCheck
ContinuousMemCheck: Tabby/vendor/raylib-tmx/CMakeFiles/ContinuousMemCheck.dir/build.make
.PHONY : ContinuousMemCheck

# Rule to build all files generated by this target.
Tabby/vendor/raylib-tmx/CMakeFiles/ContinuousMemCheck.dir/build: ContinuousMemCheck
.PHONY : Tabby/vendor/raylib-tmx/CMakeFiles/ContinuousMemCheck.dir/build

Tabby/vendor/raylib-tmx/CMakeFiles/ContinuousMemCheck.dir/clean:
	cd /home/tabby/Projects/C++/TabbyRay/build/Tabby/vendor/raylib-tmx && $(CMAKE_COMMAND) -P CMakeFiles/ContinuousMemCheck.dir/cmake_clean.cmake
.PHONY : Tabby/vendor/raylib-tmx/CMakeFiles/ContinuousMemCheck.dir/clean

Tabby/vendor/raylib-tmx/CMakeFiles/ContinuousMemCheck.dir/depend:
	cd /home/tabby/Projects/C++/TabbyRay/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tabby/Projects/C++/TabbyRay /home/tabby/Projects/C++/TabbyRay/Tabby/vendor/raylib-tmx /home/tabby/Projects/C++/TabbyRay/build /home/tabby/Projects/C++/TabbyRay/build/Tabby/vendor/raylib-tmx /home/tabby/Projects/C++/TabbyRay/build/Tabby/vendor/raylib-tmx/CMakeFiles/ContinuousMemCheck.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : Tabby/vendor/raylib-tmx/CMakeFiles/ContinuousMemCheck.dir/depend


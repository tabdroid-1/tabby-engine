# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.26

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
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.26.4/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.26.4/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/tabby/Desktop/Projects/C++/Tabby

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/tabby/Desktop/Projects/C++/Tabby/build

# Utility rule file for ContinuousBuild.

# Include any custom commands dependencies for this target.
include Tabby/vendor/yaml-cpp/CMakeFiles/ContinuousBuild.dir/compiler_depend.make

# Include the progress variables for this target.
include Tabby/vendor/yaml-cpp/CMakeFiles/ContinuousBuild.dir/progress.make

Tabby/vendor/yaml-cpp/CMakeFiles/ContinuousBuild:
	cd /Users/tabby/Desktop/Projects/C++/Tabby/build/Tabby/vendor/yaml-cpp && /opt/homebrew/Cellar/cmake/3.26.4/bin/ctest -D ContinuousBuild

ContinuousBuild: Tabby/vendor/yaml-cpp/CMakeFiles/ContinuousBuild
ContinuousBuild: Tabby/vendor/yaml-cpp/CMakeFiles/ContinuousBuild.dir/build.make
.PHONY : ContinuousBuild

# Rule to build all files generated by this target.
Tabby/vendor/yaml-cpp/CMakeFiles/ContinuousBuild.dir/build: ContinuousBuild
.PHONY : Tabby/vendor/yaml-cpp/CMakeFiles/ContinuousBuild.dir/build

Tabby/vendor/yaml-cpp/CMakeFiles/ContinuousBuild.dir/clean:
	cd /Users/tabby/Desktop/Projects/C++/Tabby/build/Tabby/vendor/yaml-cpp && $(CMAKE_COMMAND) -P CMakeFiles/ContinuousBuild.dir/cmake_clean.cmake
.PHONY : Tabby/vendor/yaml-cpp/CMakeFiles/ContinuousBuild.dir/clean

Tabby/vendor/yaml-cpp/CMakeFiles/ContinuousBuild.dir/depend:
	cd /Users/tabby/Desktop/Projects/C++/Tabby/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/tabby/Desktop/Projects/C++/Tabby /Users/tabby/Desktop/Projects/C++/Tabby/Tabby/vendor/yaml-cpp /Users/tabby/Desktop/Projects/C++/Tabby/build /Users/tabby/Desktop/Projects/C++/Tabby/build/Tabby/vendor/yaml-cpp /Users/tabby/Desktop/Projects/C++/Tabby/build/Tabby/vendor/yaml-cpp/CMakeFiles/ContinuousBuild.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Tabby/vendor/yaml-cpp/CMakeFiles/ContinuousBuild.dir/depend


# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

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
CMAKE_SOURCE_DIR = /tmp/tmp.45aQV9iUS1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /tmp/tmp.45aQV9iUS1/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/sem_Ants.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/sem_Ants.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/sem_Ants.dir/flags.make

CMakeFiles/sem_Ants.dir/main.c.o: CMakeFiles/sem_Ants.dir/flags.make
CMakeFiles/sem_Ants.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.45aQV9iUS1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/sem_Ants.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/sem_Ants.dir/main.c.o -c /tmp/tmp.45aQV9iUS1/main.c

CMakeFiles/sem_Ants.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/sem_Ants.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tmp/tmp.45aQV9iUS1/main.c > CMakeFiles/sem_Ants.dir/main.c.i

CMakeFiles/sem_Ants.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/sem_Ants.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tmp/tmp.45aQV9iUS1/main.c -o CMakeFiles/sem_Ants.dir/main.c.s

# Object files for target sem_Ants
sem_Ants_OBJECTS = \
"CMakeFiles/sem_Ants.dir/main.c.o"

# External object files for target sem_Ants
sem_Ants_EXTERNAL_OBJECTS =

sem_Ants: CMakeFiles/sem_Ants.dir/main.c.o
sem_Ants: CMakeFiles/sem_Ants.dir/build.make
sem_Ants: CMakeFiles/sem_Ants.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/tmp/tmp.45aQV9iUS1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable sem_Ants"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/sem_Ants.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/sem_Ants.dir/build: sem_Ants

.PHONY : CMakeFiles/sem_Ants.dir/build

CMakeFiles/sem_Ants.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/sem_Ants.dir/cmake_clean.cmake
.PHONY : CMakeFiles/sem_Ants.dir/clean

CMakeFiles/sem_Ants.dir/depend:
	cd /tmp/tmp.45aQV9iUS1/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tmp/tmp.45aQV9iUS1 /tmp/tmp.45aQV9iUS1 /tmp/tmp.45aQV9iUS1/cmake-build-debug /tmp/tmp.45aQV9iUS1/cmake-build-debug /tmp/tmp.45aQV9iUS1/cmake-build-debug/CMakeFiles/sem_Ants.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/sem_Ants.dir/depend


# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

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


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/mtm/cmake-3.17.0-Linux-x86_64/bin/cmake

# The command to remove a file.
RM = /home/mtm/cmake-3.17.0-Linux-x86_64/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/rotem.green/mtm/ex1/wet

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/rotem.green/mtm/ex1/wet/build

# Include any dependencies generated for this target.
include CMakeFiles/map_prog.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/map_prog.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/map_prog.dir/flags.make

CMakeFiles/map_prog.dir/map.c.o: CMakeFiles/map_prog.dir/flags.make
CMakeFiles/map_prog.dir/map.c.o: ../map.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rotem.green/mtm/ex1/wet/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/map_prog.dir/map.c.o"
	/usr/local/bin/x86_64-unknown-linux-gnu-gcc-5.5.0 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/map_prog.dir/map.c.o   -c /home/rotem.green/mtm/ex1/wet/map.c

CMakeFiles/map_prog.dir/map.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/map_prog.dir/map.c.i"
	/usr/local/bin/x86_64-unknown-linux-gnu-gcc-5.5.0 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/rotem.green/mtm/ex1/wet/map.c > CMakeFiles/map_prog.dir/map.c.i

CMakeFiles/map_prog.dir/map.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/map_prog.dir/map.c.s"
	/usr/local/bin/x86_64-unknown-linux-gnu-gcc-5.5.0 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/rotem.green/mtm/ex1/wet/map.c -o CMakeFiles/map_prog.dir/map.c.s

CMakeFiles/map_prog.dir/map_example_test.c.o: CMakeFiles/map_prog.dir/flags.make
CMakeFiles/map_prog.dir/map_example_test.c.o: ../map_example_test.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rotem.green/mtm/ex1/wet/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/map_prog.dir/map_example_test.c.o"
	/usr/local/bin/x86_64-unknown-linux-gnu-gcc-5.5.0 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/map_prog.dir/map_example_test.c.o   -c /home/rotem.green/mtm/ex1/wet/map_example_test.c

CMakeFiles/map_prog.dir/map_example_test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/map_prog.dir/map_example_test.c.i"
	/usr/local/bin/x86_64-unknown-linux-gnu-gcc-5.5.0 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/rotem.green/mtm/ex1/wet/map_example_test.c > CMakeFiles/map_prog.dir/map_example_test.c.i

CMakeFiles/map_prog.dir/map_example_test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/map_prog.dir/map_example_test.c.s"
	/usr/local/bin/x86_64-unknown-linux-gnu-gcc-5.5.0 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/rotem.green/mtm/ex1/wet/map_example_test.c -o CMakeFiles/map_prog.dir/map_example_test.c.s

# Object files for target map_prog
map_prog_OBJECTS = \
"CMakeFiles/map_prog.dir/map.c.o" \
"CMakeFiles/map_prog.dir/map_example_test.c.o"

# External object files for target map_prog
map_prog_EXTERNAL_OBJECTS =

map_prog: CMakeFiles/map_prog.dir/map.c.o
map_prog: CMakeFiles/map_prog.dir/map_example_test.c.o
map_prog: CMakeFiles/map_prog.dir/build.make
map_prog: CMakeFiles/map_prog.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/rotem.green/mtm/ex1/wet/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable map_prog"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/map_prog.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/map_prog.dir/build: map_prog

.PHONY : CMakeFiles/map_prog.dir/build

CMakeFiles/map_prog.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/map_prog.dir/cmake_clean.cmake
.PHONY : CMakeFiles/map_prog.dir/clean

CMakeFiles/map_prog.dir/depend:
	cd /home/rotem.green/mtm/ex1/wet/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rotem.green/mtm/ex1/wet /home/rotem.green/mtm/ex1/wet /home/rotem.green/mtm/ex1/wet/build /home/rotem.green/mtm/ex1/wet/build /home/rotem.green/mtm/ex1/wet/build/CMakeFiles/map_prog.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/map_prog.dir/depend

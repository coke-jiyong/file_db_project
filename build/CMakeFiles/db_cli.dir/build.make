# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ubuntu/file_db_project

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ubuntu/file_db_project/build

# Include any dependencies generated for this target.
include CMakeFiles/db_cli.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/db_cli.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/db_cli.dir/flags.make

CMakeFiles/db_cli.dir/src/main.c.o: CMakeFiles/db_cli.dir/flags.make
CMakeFiles/db_cli.dir/src/main.c.o: ../src/main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/file_db_project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/db_cli.dir/src/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/db_cli.dir/src/main.c.o   -c /home/ubuntu/file_db_project/src/main.c

CMakeFiles/db_cli.dir/src/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/db_cli.dir/src/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/ubuntu/file_db_project/src/main.c > CMakeFiles/db_cli.dir/src/main.c.i

CMakeFiles/db_cli.dir/src/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/db_cli.dir/src/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/ubuntu/file_db_project/src/main.c -o CMakeFiles/db_cli.dir/src/main.c.s

# Object files for target db_cli
db_cli_OBJECTS = \
"CMakeFiles/db_cli.dir/src/main.c.o"

# External object files for target db_cli
db_cli_EXTERNAL_OBJECTS =

db_cli: CMakeFiles/db_cli.dir/src/main.c.o
db_cli: CMakeFiles/db_cli.dir/build.make
db_cli: libfile_db.a
db_cli: CMakeFiles/db_cli.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ubuntu/file_db_project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable db_cli"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/db_cli.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/db_cli.dir/build: db_cli

.PHONY : CMakeFiles/db_cli.dir/build

CMakeFiles/db_cli.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/db_cli.dir/cmake_clean.cmake
.PHONY : CMakeFiles/db_cli.dir/clean

CMakeFiles/db_cli.dir/depend:
	cd /home/ubuntu/file_db_project/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ubuntu/file_db_project /home/ubuntu/file_db_project /home/ubuntu/file_db_project/build /home/ubuntu/file_db_project/build /home/ubuntu/file_db_project/build/CMakeFiles/db_cli.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/db_cli.dir/depend


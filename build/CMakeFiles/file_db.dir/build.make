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
include CMakeFiles/file_db.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/file_db.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/file_db.dir/flags.make

CMakeFiles/file_db.dir/src/sys_utils.c.o: CMakeFiles/file_db.dir/flags.make
CMakeFiles/file_db.dir/src/sys_utils.c.o: ../src/sys_utils.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/file_db_project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/file_db.dir/src/sys_utils.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/file_db.dir/src/sys_utils.c.o   -c /home/ubuntu/file_db_project/src/sys_utils.c

CMakeFiles/file_db.dir/src/sys_utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/file_db.dir/src/sys_utils.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/ubuntu/file_db_project/src/sys_utils.c > CMakeFiles/file_db.dir/src/sys_utils.c.i

CMakeFiles/file_db.dir/src/sys_utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/file_db.dir/src/sys_utils.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/ubuntu/file_db_project/src/sys_utils.c -o CMakeFiles/file_db.dir/src/sys_utils.c.s

CMakeFiles/file_db.dir/src/user_db.c.o: CMakeFiles/file_db.dir/flags.make
CMakeFiles/file_db.dir/src/user_db.c.o: ../src/user_db.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/file_db_project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/file_db.dir/src/user_db.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/file_db.dir/src/user_db.c.o   -c /home/ubuntu/file_db_project/src/user_db.c

CMakeFiles/file_db.dir/src/user_db.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/file_db.dir/src/user_db.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/ubuntu/file_db_project/src/user_db.c > CMakeFiles/file_db.dir/src/user_db.c.i

CMakeFiles/file_db.dir/src/user_db.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/file_db.dir/src/user_db.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/ubuntu/file_db_project/src/user_db.c -o CMakeFiles/file_db.dir/src/user_db.c.s

CMakeFiles/file_db.dir/src/hkdf.c.o: CMakeFiles/file_db.dir/flags.make
CMakeFiles/file_db.dir/src/hkdf.c.o: ../src/hkdf.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/file_db_project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/file_db.dir/src/hkdf.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/file_db.dir/src/hkdf.c.o   -c /home/ubuntu/file_db_project/src/hkdf.c

CMakeFiles/file_db.dir/src/hkdf.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/file_db.dir/src/hkdf.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/ubuntu/file_db_project/src/hkdf.c > CMakeFiles/file_db.dir/src/hkdf.c.i

CMakeFiles/file_db.dir/src/hkdf.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/file_db.dir/src/hkdf.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/ubuntu/file_db_project/src/hkdf.c -o CMakeFiles/file_db.dir/src/hkdf.c.s

# Object files for target file_db
file_db_OBJECTS = \
"CMakeFiles/file_db.dir/src/sys_utils.c.o" \
"CMakeFiles/file_db.dir/src/user_db.c.o" \
"CMakeFiles/file_db.dir/src/hkdf.c.o"

# External object files for target file_db
file_db_EXTERNAL_OBJECTS =

libfile_db.a: CMakeFiles/file_db.dir/src/sys_utils.c.o
libfile_db.a: CMakeFiles/file_db.dir/src/user_db.c.o
libfile_db.a: CMakeFiles/file_db.dir/src/hkdf.c.o
libfile_db.a: CMakeFiles/file_db.dir/build.make
libfile_db.a: CMakeFiles/file_db.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ubuntu/file_db_project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking C static library libfile_db.a"
	$(CMAKE_COMMAND) -P CMakeFiles/file_db.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/file_db.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/file_db.dir/build: libfile_db.a

.PHONY : CMakeFiles/file_db.dir/build

CMakeFiles/file_db.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/file_db.dir/cmake_clean.cmake
.PHONY : CMakeFiles/file_db.dir/clean

CMakeFiles/file_db.dir/depend:
	cd /home/ubuntu/file_db_project/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ubuntu/file_db_project /home/ubuntu/file_db_project /home/ubuntu/file_db_project/build /home/ubuntu/file_db_project/build /home/ubuntu/file_db_project/build/CMakeFiles/file_db.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/file_db.dir/depend


# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_SOURCE_DIR = /root/workspace/intelligentcar_T710_c++_v1/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/workspace/intelligentcar_T710_c++_v1/build

# Include any dependencies generated for this target.
include CMakeFiles/cross_recognition.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/cross_recognition.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/cross_recognition.dir/flags.make

CMakeFiles/cross_recognition.dir/demo/cross_recognition.cpp.o: CMakeFiles/cross_recognition.dir/flags.make
CMakeFiles/cross_recognition.dir/demo/cross_recognition.cpp.o: /root/workspace/intelligentcar_T710_c++_v1/src/demo/cross_recognition.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/workspace/intelligentcar_T710_c++_v1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/cross_recognition.dir/demo/cross_recognition.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/cross_recognition.dir/demo/cross_recognition.cpp.o -c /root/workspace/intelligentcar_T710_c++_v1/src/demo/cross_recognition.cpp

CMakeFiles/cross_recognition.dir/demo/cross_recognition.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cross_recognition.dir/demo/cross_recognition.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/workspace/intelligentcar_T710_c++_v1/src/demo/cross_recognition.cpp > CMakeFiles/cross_recognition.dir/demo/cross_recognition.cpp.i

CMakeFiles/cross_recognition.dir/demo/cross_recognition.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cross_recognition.dir/demo/cross_recognition.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/workspace/intelligentcar_T710_c++_v1/src/demo/cross_recognition.cpp -o CMakeFiles/cross_recognition.dir/demo/cross_recognition.cpp.s

CMakeFiles/cross_recognition.dir/demo/cross_recognition.cpp.o.requires:

.PHONY : CMakeFiles/cross_recognition.dir/demo/cross_recognition.cpp.o.requires

CMakeFiles/cross_recognition.dir/demo/cross_recognition.cpp.o.provides: CMakeFiles/cross_recognition.dir/demo/cross_recognition.cpp.o.requires
	$(MAKE) -f CMakeFiles/cross_recognition.dir/build.make CMakeFiles/cross_recognition.dir/demo/cross_recognition.cpp.o.provides.build
.PHONY : CMakeFiles/cross_recognition.dir/demo/cross_recognition.cpp.o.provides

CMakeFiles/cross_recognition.dir/demo/cross_recognition.cpp.o.provides.build: CMakeFiles/cross_recognition.dir/demo/cross_recognition.cpp.o


# Object files for target cross_recognition
cross_recognition_OBJECTS = \
"CMakeFiles/cross_recognition.dir/demo/cross_recognition.cpp.o"

# External object files for target cross_recognition
cross_recognition_EXTERNAL_OBJECTS =

cross_recognition: CMakeFiles/cross_recognition.dir/demo/cross_recognition.cpp.o
cross_recognition: CMakeFiles/cross_recognition.dir/build.make
cross_recognition: /usr/local/lib/libopencv_shape.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_stitching.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_superres.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_videostab.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_aruco.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_bgsegm.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_bioinspired.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_ccalib.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_dnn_objdetect.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_dpm.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_face.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_freetype.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_fuzzy.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_hfs.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_img_hash.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_line_descriptor.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_optflow.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_reg.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_rgbd.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_saliency.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_stereo.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_structured_light.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_surface_matching.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_tracking.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_ximgproc.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_xobjdetect.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_xphoto.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_highgui.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_videoio.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_phase_unwrapping.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_video.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_datasets.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_plot.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_text.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_dnn.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_ml.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_imgcodecs.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_objdetect.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_calib3d.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_features2d.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_flann.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_photo.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_imgproc.so.3.4.8
cross_recognition: /usr/local/lib/libopencv_core.so.3.4.8
cross_recognition: CMakeFiles/cross_recognition.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/workspace/intelligentcar_T710_c++_v1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable cross_recognition"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cross_recognition.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/cross_recognition.dir/build: cross_recognition

.PHONY : CMakeFiles/cross_recognition.dir/build

CMakeFiles/cross_recognition.dir/requires: CMakeFiles/cross_recognition.dir/demo/cross_recognition.cpp.o.requires

.PHONY : CMakeFiles/cross_recognition.dir/requires

CMakeFiles/cross_recognition.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/cross_recognition.dir/cmake_clean.cmake
.PHONY : CMakeFiles/cross_recognition.dir/clean

CMakeFiles/cross_recognition.dir/depend:
	cd /root/workspace/intelligentcar_T710_c++_v1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/workspace/intelligentcar_T710_c++_v1/src /root/workspace/intelligentcar_T710_c++_v1/src /root/workspace/intelligentcar_T710_c++_v1/build /root/workspace/intelligentcar_T710_c++_v1/build /root/workspace/intelligentcar_T710_c++_v1/build/CMakeFiles/cross_recognition.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/cross_recognition.dir/depend


# CMakeList.txt : CMake project for AoC2022, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (Day01 "Day01.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Day01 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

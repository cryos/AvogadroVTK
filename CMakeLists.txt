# Example project building an external plugin against installed Avogadro
# library. See http://avogadro.openmolecules.net/ for more information.

# Project name - plugin name or something meaningful
project(AvogadroVTK)

# Minimum version of CMake that can be used
cmake_minimum_required(VERSION 2.8)

# Find the Avogadro library if it is installed
find_package(Avogadro 1.1.0 REQUIRED NO_MODULE)

# Find the VTK library too
find_package(VTK REQUIRED)

# Need open babel for now to get the atom colors
find_package(OpenBabel2 REQUIRED)
include_directories(${OPENBABEL2_INCLUDE_DIR})

# Set up the build environment
include(${Avogadro_USE_FILE})
include(${VTK_USE_FILE})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Build your plugin using the default options
avogadro_plugin(vtkextension "vtkextension.cpp;VTKDialog.cpp")

target_link_libraries(vtkextension ${QT_LIBRARIES} QVTK vtkVolumeRendering)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

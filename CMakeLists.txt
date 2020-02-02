cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME "harmonySEQ")

project(
  ${PROJECT_NAME}
  VERSION 0.16.1
  LANGUAGES CXX
  )

# Set a default build type if none was specified
set(default_build_type "Release")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  set(default_build_type "Debug")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

find_package(PkgConfig REQUIRED)

add_subdirectory(src)

# Data files and resources
install(
  DIRECTORY "${PROJECT_SOURCE_DIR}/style/"
  DESTINATION share/harmonySEQ/style/
  )

install(
  DIRECTORY "${PROJECT_SOURCE_DIR}/pixmaps/"
  DESTINATION share/harmonySEQ/pixmaps/
  )

install(
  FILES "${PROJECT_SOURCE_DIR}/harmonyseq-mime.xml"
  DESTINATION share/mime/packages/
  )

configure_file(harmonyseq.desktop.in harmonyseq.desktop)
install(
  FILES "${PROJECT_BINARY_DIR}/harmonyseq.desktop"
  DESTINATION share/applications/
  )

# Debian package
SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libgtkmm-3.0-1v5, libglibmm-2.4-1v5, libasound2, liblo7, libstdc++6, libc6")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MIDI sequencer for live performances")
# NOTE: The description for debian packages is very sensitive to whitespaces.
SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "\
 MIDI software sequencer designed for live performances and jams.
 harmonySEQ operates a number of synchronized sequencers, each with its
 own pattern, repeating looped melodies of different length, duration
 and MIDI channel.")
SET(CPACK_DEBIAN_PACKAGE_SECTION "sound")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Rafał Cieślak <rafalcieslak256@gmail.com>")
SET(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/rafalcieslak/harmonySEQ")
INCLUDE(CPack)

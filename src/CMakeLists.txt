file(
  GLOB SOURCES
  CONFIGURE_DEPENDS
  *.cpp
  )

pkg_check_modules(GTKMM gtkmm-3.0 REQUIRED)
pkg_check_modules(GLIBMM glibmm-2.4 REQUIRED)
pkg_check_modules(LIBLO liblo)
find_package(ALSA REQUIRED)

add_executable(
  harmonySEQ
  ${SOURCES}
  )

configure_file(config.h.in config.h)

if(NOT LIBLO_FOUND)
  message(STATUS "liblo not found, disabling OSC support.")
  target_compile_definitions(
    harmonySEQ PUBLIC
    -DDISABLE_OSC)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  message(STATUS "Release mode")
  target_compile_definitions(
    harmonySEQ PUBLIC
    -DRELEASE)
endif()

# Use C++17
set_target_properties(
  harmonySEQ PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED True)

target_include_directories(
  harmonySEQ PUBLIC
  # version.h will be generated into binary dir
  ${PROJECT_BINARY_DIR}/src
  # Dependencies
  ${ALSA_INCLUDE_DIRS}
  ${GTKMM_INCLUDE_DIRS}
  ${LIBLO_INCLUDE_DIRS}
  )

target_compile_options(
  harmonySEQ PUBLIC
  # Glib has deprecated some interfaces that are still used by
  # GTK. This generates a ton of warnings we can't resolve...
  -Wno-deprecated-declarations
  )

target_link_libraries(
  harmonySEQ
  ${ALSA_LIBRARIES}
  ${GTKMM_LIBRARIES}
  ${GLIBMM_LIBRARIES}
  ${LIBLO_LIBRARIES}
  )

set_target_properties(
  harmonySEQ PROPERTIES
  LINK_FLAGS_RELEASE -s)

# When building inplace directory, place the binary in the root.
set_target_properties(
  harmonySEQ
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
  )

# When installing, move harmonySEQ binary to the bin directory
install(
  TARGETS harmonySEQ
  RUNTIME DESTINATION bin
  )

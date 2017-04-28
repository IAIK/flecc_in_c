cmake_minimum_required(VERSION 3.1.0)

project(flecc_in_c C)
set(VERSION_MAJOR "1")
set(VERSION_MINOR "1")
set(VERSION_PATCH "1")

#enable_language(ASM)

set(DEFAULT_BUILD_TYPE MinSizeRel)
set(DEFAULT_C_FLAGS_GCC "-Wall -Wextra -pedantic -Wno-unused-parameter")
set(DOXYGEN_CONFIG_IN_FILE "${PROJECT_SOURCE_DIR}/doc/template.doxy.in")

set(CMAKE_C_STANDARD     99)
set(CMAKE_C_EXTENSIONS   OFF)

#------------------------------------------------------------------------------
# General config
#------------------------------------------------------------------------------
# add the project internal module directory to the module path
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

include(forceOutOfSourceBuild)       # prohibit in-source builds
include(setDefaultSettings)          # processes and applies the DEFAULT variables
include(messages)                    # helper functions for messages
include(clangformat)                 # add support for batch code reformating
include(doxygenSupport)              # adds doxygen support
include(installSupport)              # add uninstall target and install helpers
include(testCoverage)                # adds coverage test targets
include(testing)                     # enables ctest and generates suite and check
include(versionSupport)              # build up version strings

#------------------------------------------------------------------------------
# Project config
#------------------------------------------------------------------------------
add_subdirectory(include)
add_subdirectory(src)

if(NOT SUB_PROJECT)
  set(CPACK_GENERATOR "TGZ")
  set(CPACK_SOURCE_GENERATOR "TGZ")
  set(CPACK_SOURCE_IGNORE_FILES "${PROJECT_BINARY_DIR}/" "/[.]git/")
  include(CPack)

  option(INSTALL_DOXYGEN "Install the doxygen documentation with the library." OFF)
  if(INSTALL_DOXYGEN)
    # hack to build the doxygen documentation before installing
    install_lib(CODE "execute_process(COMMAND ${CMAKE_COMMAND} --build . --target doxygen WORKING_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}\")")
    install_lib(DIRECTORY "${DOXYGEN_OUTPUT_DIR}/" DESTINATION "share/doc/flecc_in_c-${VERSION_FULL}/doxygen")
  endif()

  add_subdirectory(testrunner EXCLUDE_FROM_ALL)
  add_subdirectory(tests EXCLUDE_FROM_ALL)

  # register all testfiles as tests in ctest
  #-----------------------------------------
  file(GLOB FleccLibSources "testfiles/*.tst")
  foreach(loop ${FleccLibSources})
    get_filename_component(name "${loop}" NAME_WE)
    add_stdin_test("${name}" testrunner "${loop}")
  endforeach()
endif()

#------------------------------------------------------------------------------
# Print help dialog (assuming make based build system)
#------------------------------------------------------------------------------
help_msg("---------------------------------------------------------------------")
help_msg("Configuration for ${CMAKE_PROJECT_NAME} ${VERSION_FULL} done.")
help_msg("INSTALL PREFIX:      ${CMAKE_INSTALL_PREFIX}")
help_msg("BUILD TYPE:          ${CMAKE_BUILD_TYPE}")
help_msg("")
help_msg("Available targets:")
help_target(check          "Build and execute the test suite.")
help_target(coverage       "Reset Counters + run tests + generate report.")
help_target(covGenerate    "Analyze counter files and generate report.")
help_target(covReset       "Delete coverate counter files.")
help_target(doxygen        "Build doxygen documentation.")
help_target(format         "Format the source files using clang-format.")
help_target(install        "Installing the library.")
help_target(package        "Build and pack the library.")
help_target(package_source "Pack the source directory for distribution.")
help_target(suite          "Build the test suite.")
help_target(uninstall      "Uninstall the previously installed library.")
help_msg("---------------------------------------------------------------------")

cmake_minimum_required(VERSION 3.0.0)

project(flecc_in_c C)
set(VERSION_MAJOR "1")
set(VERSION_MINOR "1")
set(VERSION_PATCH "0")

enable_language(ASM)

set(DEFAULT_BUILD_TYPE MinSizeRel)

set(DEFAULT_C_FLAGS_GCC "-Wall")

# set default Architecture
if(NOT ARCHITECTURE)
  set(ARCHITECTURE "ARCH_X86"
  CACHE STRING "ARCH_X86, ARCH_X86_64, ARCH_CORTEXA8, ARCH_CORTEXA9, ARCH_CORTEXA11,..." FORCE)
endif()

set(DOXYGEN_CONFIG_IN_FILE "${PROJECT_SOURCE_DIR}/doc/template.doxy.in")

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
set(CPACK_GENERATOR "TGZ")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES "${PROJECT_BINARY_DIR}/" "/[.]git/")
include(CPack)

find_package( StandardMathLibrary REQUIRED )

# build the flecc_in_c library
#-----------------------------------------
file(GLOB_RECURSE FleccLibSources "flecc_in_c/*.c")
set(LIBNAME "${CMAKE_PROJECT_NAME}-${ARCHITECTURE}")
add_library("${LIBNAME}" ${FleccLibSources})
target_compile_definitions("${LIBNAME}" INTERFACE "ARCHITECTURE=${ARCHITECTURE}")
target_link_libraries("${LIBNAME}" ${STANDARD_MATH_LIBRARY})
target_include_directories("${LIBNAME}" PRIVATE "flecc_in_c"
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
         $<INSTALL_INTERFACE:include>)

install_lib(TARGETS "${LIBNAME}" EXPORT "${CMAKE_PROJECT_NAME}" DESTINATION "lib${LIBDIR_INSTALL_SUFFIX}")
install_lib(DIRECTORY "flecc_in_c/" DESTINATION "include/flecc_in_c"
                         FILES_MATCHING PATTERN "*.h")
export_and_install_config("${CMAKE_PROJECT_NAME}")

option(INSTALL_DOXYGEN "Install the doxygen documentation with the library." OFF)
if(INSTALL_DOXYGEN)
  # hack to build the doxygen documentation before installing
  install_lib(CODE "execute_process(COMMAND ${CMAKE_COMMAND} --build . --target doxygen WORKING_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}\")")
  install_lib(DIRECTORY "${DOXYGEN_OUTPUT_DIR}/" DESTINATION "share/doc/flecc_in_c-${VERSION_FULL}/doxygen")
endif()

# build the test runner
#-----------------------------------------
file(GLOB TestrunnerSources "testrunner/*.c")
add_executable("testrunner" EXCLUDE_FROM_ALL ${TestrunnerSources})
target_link_libraries("testrunner" "${LIBNAME}")
add_to_suite("testrunner")

# build the test ecdlp
#-----------------------------------------
find_package( Threads REQUIRED )
file(GLOB TestrunnerSources "test_ecdlp/*.c")
add_executable("test_ecdlp" EXCLUDE_FROM_ALL ${TestrunnerSources})
target_link_libraries("test_ecdlp" "${LIBNAME}" ${CMAKE_THREAD_LIBS_INIT})

# generate a test target
#-----------------------------------------
file(GLOB FleccLibSources "tests/*.tst")
foreach(loop ${FleccLibSources})
  get_filename_component(name "${loop}" NAME_WE)
  add_stdin_test("${name}" "testrunner" "${loop}")
endforeach()

# register source files for reformating
set(FormatBlacklist "${CMAKE_SOURCE_DIR}/flecc_in_c/utils/param_const.c")
file(GLOB_RECURSE Sources "flecc_in_c/*.h" "flecc_in_c/*.c" "testrunner/*.h" "testrunner/*.c")
list(REMOVE_ITEM Sources ${FormatBlacklist})
add_file_to_format(${Sources})

#------------------------------------------------------------------------------
# Print help dialog (assuming make based build system)
#------------------------------------------------------------------------------
help_msg("---------------------------------------------------------------------")
help_msg("Configuration for ${CMAKE_PROJECT_NAME} ${VERSION_FULL} done.")
help_msg("INSTALL PREFIX:      ${CMAKE_INSTALL_PREFIX}")
help_msg("BUILD TYPE:          ${CMAKE_BUILD_TYPE}")
help_msg("")
help_msg("Available targets:")
help_target(install        "Installing the library.")
help_target(uninstall      "Uninstall the previously installed library.")
help_target(package        "Build and pack the library.")
help_target(package_source "Pack the source directory for distribution.")
help_target(suite          "Build the test suite.")
help_target(check          "Build and execute the test suite.")
help_target(doxygen        "Build doxygen documentation.")
help_target(covReset       "Delete coverate counter files.")
help_target(covGenerate    "Analyze counter files and generate report.")
help_target(coverage       "Reset Counters + run tests + generate report.")
help_target(format         "Format the source files using clang-format.")
help_msg("---------------------------------------------------------------------")

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.10) 

# set project name
PROJECT(flecc_in_c C)
SET(VERSION_MAJOR "1")
SET(VERSION_MINOR "0")


SET(DEFAULT_BUILD_TYPE MinSizeRel)

SET(DEFAULT_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/static")
SET(DEFAULT_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
SET(DEFAULT_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

SET(DEFAULT_C_FLAGS_GCC "-Wall")
SET(DEFAULT_ECLIPSE_SUPPORT_STATE "ON")
SET(DEFAULT_32BIT_COMPILE_STATE "OFF")

# set default Architecture
IF(NOT ARCHITECTURE)
  SET(ARCHITECTURE "ARCH_X86"
  CACHE STRING "ARCH_X86, ARCH_X86_64, ARCH_CORTEXA8, ARCH_CORTEXA9, ARCH_CORTEXA11,..." FORCE)
ENDIF(NOT ARCHITECTURE)

SET(DOXYGEN_CONFIG_IN_FILE "${PROJECT_SOURCE_DIR}/doc/template.doxy.in")

#------------------------------------------------------------------------------
# General config
#------------------------------------------------------------------------------
# add the project internal module directory to the module path
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

INCLUDE(clangformat)                 # add support for batch code reformating
INCLUDE(doxygenSupport)              # adds doxygen support
INCLUDE(forceOutOfSourceBuild)       # prohibit in-source builds
INCLUDE(installSupport)              # add uninstall target and install helpers
INCLUDE(setDefaultSettings)          # processes and applies the DEFAULT variables
INCLUDE(testCoverage)                # adds coverage test targets
INCLUDE(testing)                     # enables ctest and generates suite and check
INCLUDE(versionSupport)              # build up version strings

#------------------------------------------------------------------------------
# Project config
#------------------------------------------------------------------------------
INCLUDE_DIRECTORIES( "src" "testrunner" )

set(CPACK_GENERATOR "TGZ")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES "${PROJECT_BINARY_DIR}/" "/[.]git/")
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
INCLUDE(CPack)

# build the flecc_in_c library
#-----------------------------------------
FILE(GLOB_RECURSE FleccLibSources "src/*.c")
ADD_LIBRARY("flecc_in_c" ${sourceFile} ${FleccLibSources})
SET_TARGET_PROPERTIES("flecc_in_c" PROPERTIES
        COMPILE_FLAGS "-DARCHITECTURE=${ARCHITECTURE}")

INSTALL(TARGETS "flecc_in_c" DESTINATION "lib")
INSTALL(DIRECTORY "src/" DESTINATION "include/flecc_in_c"
                         FILES_MATCHING PATTERN "*.h")

OPTION(INSTALL_DOXYGEN "Install the doxygen documentation with the library." OFF)
IF(INSTALL_DOXYGEN)
  # hack to build the doxygen documentation before installing
  INSTALL(CODE "execute_process(COMMAND ${CMAKE_COMMAND} --build . --target doxygen WORKING_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}\")")
  INSTALL(DIRECTORY "${DOXYGEN_OUTPUT_DIR}/" DESTINATION "share/doc/flecc_in_c-${VERSION_SHORT}/doxygen")
ENDIF(INSTALL_DOXYGEN)

# build the test runner
#-----------------------------------------
FILE(GLOB TestrunnerSources "testrunner/*.c")
ADD_EXECUTABLE("testrunner" EXCLUDE_FROM_ALL ${TestrunnerSources})
SET_TARGET_PROPERTIES("testrunner" PROPERTIES
  COMPILE_FLAGS "-DARCHITECTURE=${ARCHITECTURE}")
TARGET_LINK_LIBRARIES("testrunner" "flecc_in_c")
add_to_suite("testrunner")

# generate a test target
#-----------------------------------------
FILE(GLOB FleccLibSources "tests/*.tst")
FOREACH(loop ${FleccLibSources})
  get_filename_component(name "${loop}" NAME_WE)
  add_stdin_test("${name}" "testrunner" "${loop}")
ENDFOREACH()

# register source files for reformating
SET(FormatBlacklist "${CMAKE_SOURCE_DIR}/src/utils/param_const.c")
FILE(GLOB_RECURSE Sources "src/*.h" "src/*.c" "testrunner/*.h" "testrunner/*.c")
LIST(REMOVE_ITEM Sources ${FormatBlacklist})
add_file_to_format(${Sources})

#------------------------------------------------------------------------------
# Print help dialog (assuming make based build system)
#------------------------------------------------------------------------------
MESSAGE(STATUS "---------------------------------------------------------------------")
MESSAGE(STATUS "Configuration for ${CMAKE_PROJECT_NAME}-${VERSION_FULL} done.")
MESSAGE(STATUS "INSTALL PREFIX:    ${CMAKE_INSTALL_PREFIX}")
MESSAGE(STATUS "BUILD TYPE:        ${CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "")
MESSAGE(STATUS "Available targets:")
MESSAGE(STATUS "  all..............Build the library.")
MESSAGE(STATUS "  install..........Installing the library.")
MESSAGE(STATUS "  uninstall........Uninstall the previously installed library.")
MESSAGE(STATUS "  package..........Build and pack the library.")
MESSAGE(STATUS "  package_source...Pack the source directory for distribution.")
MESSAGE(STATUS "  suite............Build the test suite.")
MESSAGE(STATUS "  check............Build and execute the test suite.")
IF( DOXYGEN_FOUND )
  MESSAGE(STATUS "  doxygen..........Build doxygen documentation.")
ENDIF( )
IF( COVERAGE_TEST_POSSIBLE AND TEST_COVERAGE)
  MESSAGE(STATUS "  covReset.........Delete coverate counter files.")
  MESSAGE(STATUS "  covGenerate......Analyze counter files and generate report.")
  MESSAGE(STATUS "  coverage.........Reset Counters + run tests + generate report.")
ENDIF( )
IF( CLANGFORMAT_POSSIBLE )
  MESSAGE(STATUS "  format...........Format the source files using clang-format.")
ENDIF( )
MESSAGE(STATUS "---------------------------------------------------------------------")

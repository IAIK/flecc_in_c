CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9) 

# set project name
PROJECT(flecc_in_c C)
SET(VERSION_MAJOR "1")
SET(VERSION_MINOR "0")
SET(VERSION_PATCH "1")

#enable Assembler support
ENABLE_LANGUAGE(ASM)

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

INCLUDE(forceOutOfSourceBuild)       # prohibit in-source builds
INCLUDE(setDefaultSettings)          # processes and applies the DEFAULT variables
INCLUDE(messages)                    # helper functions for messages
INCLUDE(clangformat)                 # add support for batch code reformating
INCLUDE(doxygenSupport)              # adds doxygen support
INCLUDE(installSupport)              # add uninstall target and install helpers
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
INCLUDE(CPack)

# build the flecc_in_c library
#-----------------------------------------
FILE(GLOB_RECURSE FleccLibSources "src/*.c")
SET(LIBNAME "flecc_in_c-${ARCHITECTURE}")
ADD_LIBRARY("${LIBNAME}" ${FleccLibSources})
SET_TARGET_PROPERTIES("${LIBNAME}" PROPERTIES
        COMPILE_FLAGS "-DARCHITECTURE=${ARCHITECTURE}")

install_lib(TARGETS "${LIBNAME}" DESTINATION "lib${LIBDIR_INSTALL_SUFFIX}")
install_lib(DIRECTORY "src/" DESTINATION "include/flecc_in_c"
                         FILES_MATCHING PATTERN "*.h")

OPTION(INSTALL_DOXYGEN "Install the doxygen documentation with the library." OFF)
IF(INSTALL_DOXYGEN)
  # hack to build the doxygen documentation before installing
  install_lib(CODE "execute_process(COMMAND ${CMAKE_COMMAND} --build . --target doxygen WORKING_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}\")")
  install_lib(DIRECTORY "${DOXYGEN_OUTPUT_DIR}/" DESTINATION "share/doc/flecc_in_c-${VERSION_FULL}/doxygen")
ENDIF(INSTALL_DOXYGEN)

# build the test runner
#-----------------------------------------
FILE(GLOB TestrunnerSources "testrunner/*.c")
ADD_EXECUTABLE("testrunner" EXCLUDE_FROM_ALL ${TestrunnerSources})
SET_TARGET_PROPERTIES("testrunner" PROPERTIES
  COMPILE_FLAGS "-DARCHITECTURE=${ARCHITECTURE}")
TARGET_LINK_LIBRARIES("testrunner" "${LIBNAME}")
add_to_suite("testrunner")

# build the test ecdlp
#-----------------------------------------
FILE(GLOB TestrunnerSources "test_ecdlp/*.c")
ADD_EXECUTABLE("test_ecdlp" EXCLUDE_FROM_ALL ${TestrunnerSources})
SET_TARGET_PROPERTIES("test_ecdlp" PROPERTIES
  COMPILE_FLAGS "-DARCHITECTURE=${ARCHITECTURE}")
TARGET_LINK_LIBRARIES("test_ecdlp" "flecc_in_c" pthread)

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
HELP_MSG("---------------------------------------------------------------------")
HELP_MSG("Configuration for ${CMAKE_PROJECT_NAME} ${VERSION_FULL} done.")
HELP_MSG("INSTALL PREFIX:      ${CMAKE_INSTALL_PREFIX}")
HELP_MSG("BUILD TYPE:          ${CMAKE_BUILD_TYPE}")
HELP_MSG("")
HELP_MSG("Available targets:")
HELP_TARGET(install        "Installing the library.")
HELP_TARGET(uninstall      "Uninstall the previously installed library.")
HELP_TARGET(package        "Build and pack the library.")
HELP_TARGET(package_source "Pack the source directory for distribution.")
HELP_TARGET(suite          "Build the test suite.")
HELP_TARGET(check          "Build and execute the test suite.")
HELP_TARGET(doxygen        "Build doxygen documentation.")
HELP_TARGET(covReset       "Delete coverate counter files.")
HELP_TARGET(covGenerate    "Analyze counter files and generate report.")
HELP_TARGET(coverage       "Reset Counters + run tests + generate report.")
HELP_TARGET(format         "Format the source files using clang-format.")
HELP_MSG("---------------------------------------------------------------------")

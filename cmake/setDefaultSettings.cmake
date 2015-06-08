# This script handles the initialization of the most important and used
# cmake variables. The values are only updated when they don't exist at
# this time and default values are available. Furthermore the default values are
# exported to the cache where they can be manipulated later on.
#
# In addition to the default values toolchain variables are processed as well.
# They are automatically appended to the CMAKE flags. Therefore the default
# values are not used in this case.
# 
# Processed variables:
# * Defining of the default output directories for the different artifacts:
#   DEFAULT_ARCHIVE_OUTPUT_DIRECTORY....directory for static libs
#   DEFAULT_LIBRARY_OUTPUT_DIRECTORY....directory for dynamic/shared libs
#   DEFAULT_RUNTIME_OUTPUT_DIRECTORY....directory for executables
#
# * Defining the default build type:
#   TOOLCHAIN_BUILD_TYPE or
#   DEFAULT_BUILD_TYPE..................usually Debug or Release
#
# * Defining the C and CXX compiler flags:
#   TOOLCHAIN_C_FLAGS or
#   DEFAULT_C_FLAGS_GCC.................flags when gcc and clang is used (C)
#   DEFAULT_C_FLAGS_MSVC................flags when cl is used (C)
#   TOOLCHAIN_CXX_FLAGS or
#   DEFAULT_CXX_FLAGS_GCC...............flags when g++ and clang++ is used (C++)
#   DEFAULT_CXX_FLAGS_MSVC..............flags when cl is used (C++)
#
# * Defining the linker flags:
#   TOOLCHAIN_LINKER_FLAGS or
#   DEFAULT_EXE_LINKER_FLAGS_GCC........flags for linking executables with GCC
#   DEFAULT_EXE_LINKER_FLAGS_MSVC.......flags for linking executables with MSVC
#   DEFAULT_SHARED_LINKER_FLAGS_GCC.....flags for linking shared libs with GCC
#   DEFAULT_SHARED_LINKER_FLAGS_MSVC....flags for linking shared libs with MSVC
#
# * Enables optional features when the variable is set
#   DEFAULT_ECLIPSE_SUPPORT_STATE.......(ON/OFF) adds compiler flags for better
#                                       error parsing when using eclipse
#                                       (GCC only)
#   DEFAULT_32BIT_COMPILE_STATE.........(ON/OFF) adds compiler flags for doing
#                                       a cross compile to 32-bit while using
#                                       a 64-bit compiler (GCC only)
#
# Provided targets:
#   ---
#
# Provided macros/functions:
#   CACHE_WITH_DEFAULT..................cache the variable and use a default
#                                       value if it was empty
#   CACHE_NEW_WITH_DEFAULT..............cache the variable and use a default
#                                       value if it was undefined
#

# enable the compilation database
SET(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

# helper macro to to export a variable to the cache
# the variable is set to an default value when it was empty
# e.g. CACHE_WITH_DEFAULT(MD_ENGINE "foo" "STRING" "The used markdown engine.")
MACRO(CACHE_WITH_DEFAULT name default type description)
  IF(NOT ${name})
    SET("${name}" "${default}" CACHE "${type}" "${description}" FORCE)
  ELSE()
    SET("${name}" "${${name}}" CACHE "${type}" "${description}" FORCE)
  ENDIF()
ENDMACRO()

# helper macro to to export a variable to the cache
# the variable is set to an default value when it was undefined
# e.g. CACHE_WITH_DEFAULT(MD_ENGINE "foo" "STRING" "The used markdown engine.")
MACRO(CACHE_NEW_WITH_DEFAULT name default type description)
  IF(NOT DEFINED "${name}")
    SET("${name}" "${default}" CACHE "${type}" "${description}" FORCE)
  ELSE()
    SET("${name}" "${${name}}" CACHE "${type}" "${description}" FORCE)
  ENDIF()
ENDMACRO()

# determine if the project is built as subproject of another project
IF("SUB_PROJECT_${PROJECT_NAME}" OR NOT "${PROJECT_NAME}" STREQUAL "${CMAKE_PROJECT_NAME}")
  SET(SUB_PROJECT TRUE)
  CACHE_WITH_DEFAULT("SUB_PROJECT_${PROJECT_NAME}" "TRUE" "BOOL" "The project is built as subproject. (no install, no documentation, no format)")
ENDIF()
MARK_AS_ADVANCED("SUB_PROJECT_${PROJECT_NAME}")

#------------------------------------------------------------------------------
# Set build directories
#------------------------------------------------------------------------------
# place all the built static libraries into a common directory
CACHE_WITH_DEFAULT(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
                   "${DEFAULT_ARCHIVE_OUTPUT_DIRECTORY}"
                   "PATH"
                   "Single output directory for building all static and import libraries.")

# place all the built shared libraries into a common directory
CACHE_WITH_DEFAULT(CMAKE_LIBRARY_OUTPUT_DIRECTORY
                   "${DEFAULT_LIBRARY_OUTPUT_DIRECTORY}"
                   "PATH"
                   "Single output directory for building all shared libraries.")

# place all the built binaries into a common directory
CACHE_WITH_DEFAULT(CMAKE_RUNTIME_OUTPUT_DIRECTORY
                   "${DEFAULT_RUNTIME_OUTPUT_DIRECTORY}"
                   "PATH"
                   "Single output directory for building all executables.")

MARK_AS_ADVANCED(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
                 CMAKE_LIBRARY_OUTPUT_DIRECTORY
                 CMAKE_RUNTIME_OUTPUT_DIRECTORY)

#------------------------------------------------------------------------------
# Set build type
#------------------------------------------------------------------------------
# set the default build type to the value of ${TOOLCHAIN_BUILD_TYPE} or
# ${DEFAULT_BUILD_TYPE}
# (Values: None, Debug, Release, RelWithDebInfo, MinSizeRel)
CACHE_WITH_DEFAULT(CMAKE_BUILD_TYPE
                   "${TOOLCHAIN_BUILD_TYPE}"
                   "STRING"
                   "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel.")

CACHE_WITH_DEFAULT(CMAKE_BUILD_TYPE
                   "${DEFAULT_BUILD_TYPE}"
                   "STRING"
                   "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel.")

#------------------------------------------------------------------------------
# Set compiler flags
#------------------------------------------------------------------------------
IF("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
  SET(CLANG "1")
ENDIF()

# set flags using the toolchain variables
#----------------------------------------
IF(TOOLCHAIN_C_FLAGS)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TOOLCHAIN_C_FLAGS}")
ENDIF()

IF(TOOLCHAIN_CXX_FLAGS)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TOOLCHAIN_CXX_FLAGS}")
ENDIF()

IF(TOOLCHAIN_ASM_FLAGS)
  SET(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${TOOLCHAIN_ASM_FLAGS}")
ENDIF()

# set flags using the default variables
#----------------------------------------
IF(NOT CMAKE_C_FLAGS)
  IF((CMAKE_COMPILER_IS_GNUCC OR MINGW OR CLANG) AND DEFAULT_C_FLAGS_GCC)
    SET(CMAKE_C_FLAGS "${DEFAULT_C_FLAGS_GCC}" CACHE STRING
      "Flags used by the compiler during all build types." FORCE)
  ENDIF()

  IF(MSVC AND DEFAULT_C_FLAGS_MSVC)
    SET(CMAKE_C_FLAGS "${DEFAULT_C_FLAGS_MSVC}" CACHE STRING
      "Flags used by the compiler during all build types." FORCE)
  ENDIF()
ENDIF()

IF(NOT CMAKE_CXX_FLAGS)
  IF((CMAKE_COMPILER_IS_GNUCC OR MINGW OR CLANG) AND DEFAULT_CXX_FLAGS_GCC)
    SET(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS_GCC}" CACHE STRING
      "Flags used by the compiler during all build types." FORCE)
  ENDIF()

  IF(MSVC AND DEFAULT_CXX_FLAGS_MSVC)
    SET(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS_MSVC}" CACHE STRING
      "Flags used by the compiler during all build types." FORCE)
  ENDIF()
ENDIF()

#------------------------------------------------------------------------------
# Set linker flags
#------------------------------------------------------------------------------

# set flags using the toolchain variables
#----------------------------------------
IF(TOOLCHAIN_LINKER_FLAGS)
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${TOOLCHAIN_LINKER_FLAGS}")
ENDIF()

IF(TOOLCHAIN_LINKER_FLAGS)
  SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${TOOLCHAIN_LINKER_FLAGS}")
ENDIF()

# set flags using the default variables
#----------------------------------------
IF(NOT CMAKE_EXE_LINKER_FLAGS)
  IF((CMAKE_COMPILER_IS_GNUCC OR MINGW OR CLANG) AND DEFAULT_EXE_LINKER_FLAGS_GCC)
    SET(CMAKE_EXE_LINKER_FLAGS "${DEFAULT_EXE_LINKER_FLAGS_GCC}" CACHE STRING
      "Flags used by the linker." FORCE)
  ENDIF()

  IF(MSVC AND DEFAULT_EXE_LINKER_FLAGS_MSVC)
    SET(CMAKE_EXE_LINKER_FLAGS "${DEFAULT_EXE_LINKER_FLAGS_MSVC}" CACHE STRING
      "Flags used by the linker." FORCE)
  ENDIF()
ENDIF()

IF(NOT CMAKE_SHARED_LINKER_FLAGS)
  IF((CMAKE_COMPILER_IS_GNUCC OR MINGW OR CLANG) AND DEFAULT_SHARED_LINKER_FLAGS_GCC)
    SET(CMAKE_SHARED_LINKER_FLAGS "${DEFAULT_SHARED_LINKER_FLAGS_GCC}" CACHE STRING
      "Flags used by the linker during the creation of shared libs." FORCE)
  ENDIF()

  IF(MSVC AND DEFAULT_SHARED_LINKER_FLAGS_MSVC)
    SET(CMAKE_SHARED_LINKER_FLAGS "${DEFAULT_SHARED_LINKER_FLAGS_MSVC}" CACHE STRING
      "Flags used by the linker during the creation of shared libs." FORCE)
  ENDIF()
ENDIF()

#------------------------------------------------------------------------------
# Enable optional features
#------------------------------------------------------------------------------
# add option to enable eclipse support (GCC only)
IF(CMAKE_COMPILER_IS_GNUCC AND CMAKE_COMPILER_IS_GNUCXX AND DEFINED DEFAULT_ECLIPSE_SUPPORT_STATE)
  OPTION(ENABLE_ECLIPSE_SUPPORT "Adds compile flags for easier error parsing." "${DEFAULT_ECLIPSE_SUPPORT_STATE}")
ENDIF()

# when building with eclipse support add the specific compile flag
IF(CMAKE_COMPILER_IS_GNUCC AND CMAKE_COMPILER_IS_GNUCXX AND ENABLE_ECLIPSE_SUPPORT)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
ENDIF()

# add option to cross compile to 32-bit on a 64-bit mashine (GCC only)
IF(CMAKE_SIZEOF_VOID_P EQUAL 8 AND (CMAKE_COMPILER_IS_GNUCC OR CLANG) AND DEFINED DEFAULT_32BIT_COMPILE_STATE)
  OPTION(BUILD_32_BITS "Cross compile to 32-bit artifacts." "${DEFAULT_32BIT_COMPILE_STATE}")
ENDIF()

# when building for 32 bits add the specific compile flag (-m32 for gcc)
IF(CMAKE_SIZEOF_VOID_P EQUAL 8 AND (CMAKE_COMPILER_IS_GNUCC OR CLANG) AND BUILD_32_BITS)
  MESSAGE(STATUS "Building executables and libraries in 32 bits.")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
ENDIF()

# add an option to specifiy a library install suffix
# only on 64bit unix maschines
SET(DEFAULT_LIBDIR_INSTALL_SUFFIX_INT "")
IF(CMAKE_SIZEOF_VOID_P EQUAL 8 AND UNIX)
  SET(DEFAULT_LIBDIR_INSTALL_SUFFIX_INT "64")
ENDIF()

CACHE_NEW_WITH_DEFAULT(LIBDIR_INSTALL_SUFFIX
                       "${DEFAULT_LIBDIR_INSTALL_SUFFIX_INT}"
                       "STRING"
                       "Define suffix of library directory name (32/64)")
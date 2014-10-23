CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4) 

# set project name
PROJECT(flecc_in_c C)

# enable assembler support
ENABLE_LANGUAGE(ASM)

#SET(DEFAULT_BUILD_TYPE MinSizeRel)
SET (DEFAULT_BUILD_TYPE Debug)

SET(DEFAULT_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/static")
SET(DEFAULT_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
SET(DEFAULT_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

SET(DEFAULT_C_FLAGS_GCC "-Wall")
SET(DEFAULT_ECLIPSE_SUPPORT_STATE "ON")
SET(DEFAULT_32BIT_COMPILE_STATE "OFF")

# activate Profiling
IF (PROFILING)
    SET(CMAKE_C_FLAGS          "${CMAKE_C_FLAGS} -g -fprofile-arcs -ftest-coverage")
    SET(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS} -g -fprofile-arcs -ftest-coverage")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg -fprofile-arcs -ftest-coverage")
ENDIF(PROFILING)

# set default Architecture
IF(NOT ARCHITECTURE)
  SET(ARCHITECTURE "ARCH_X86"
  CACHE STRING "ARCH_X86, ARCH_X86_64, ARCH_CORTEXA8, ARCH_CORTEXA9, ARCH_CORTEXA11,..." FORCE)
ENDIF(NOT ARCHITECTURE)

# extend the compile flags with the needed parameters
SET(CMAKE_ASM_FLAGS        "${CMAKE_ASM_FLAGS}        ${ARCHITECTURE_ASM_FLAGS}")
SET(CMAKE_C_FLAGS          "${CMAKE_C_FLAGS}          ${ARCHITECTURE_C_FLAGS}")
SET(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS}        ${ARCHITECTURE_CXX_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ARCHITECTURE_LINKER_FLAGS}")

#------------------------------------------------------------------------------
# General config
#------------------------------------------------------------------------------
# add the project internal module directory to the module path
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

INCLUDE(setDefaultSettings)          # processes and applies the DEFAULT variables
INCLUDE(forceOutOfSourceBuild)       # prohibit in-source builds
INCLUDE(coverage)                    # generate code coverage statistics

#------------------------------------------------------------------------------
# Project config
#------------------------------------------------------------------------------
#INCLUDE_DIRECTORIES("src")

# build all source files into the target
#-----------------------------------------
FILE(GLOB FleccLibSources "src/bi/*.c" "src/eccp/*.c" "src/gfp/*.c" "src/hash/*.c" "src/protocols/*.c" 
                          "src/utils/*.c" "src/io/*.c" "src/arch/posix/io/*.c")
FILE(GLOB TestSources "src/tests/*.c")
FILE(GLOB ExecSources "src/main.c")

foreach( sourceFile ${ExecSources} )
    get_filename_component(exeName ${sourceFile} NAME_WE)
    add_executable( ${exeName} ${sourceFile} ${FleccLibSources} ${TestSources})
    # add preprocessor define for the target
    SET_TARGET_PROPERTIES("${exeName}" PROPERTIES
        COMPILE_FLAGS "-DARCHITECTURE=${ARCHITECTURE}")
endforeach( sourceFile ${ExecSources} )

add_library( "flecc_in_c" ${sourceFile} ${FleccLibSources})
    SET_TARGET_PROPERTIES("flecc_in_c" PROPERTIES
        COMPILE_FLAGS "-DARCHITECTURE=${ARCHITECTURE}")
        
#add_custom_target(secp192r1test ${DEFAULT_RUNTIME_OUTPUT_DIRECTORY}/main  < ../../../flecc_in_c_tests/flecctests/secp192r1_bi.tst)
 
#add_custom_command(OUTPUT testlog.execution
#                   COMMAND ../../executeTests.zsh)
                   
       
IF(PROFILING)
    SETUP_TARGET_FOR_COVERAGE(coverage ../../lcov_testrunner.zsh coverage)
ENDIF(PROFILING)
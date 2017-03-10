# to compile with this toolchain call cmake like this
# e.g.: cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain/arm-none-eabi-gcc.cmake ..

# some basic configurations for the target platform
SET(CPU   cortex-m0    CACHE STRING "Device name for the compiler and tools (e.g.: cortex-m0)")

INCLUDE(CMakeForceCompiler)

# specify the cross compiler
SET(CMAKE_SYSTEM_NAME Generic)
CMAKE_FORCE_C_COMPILER(arm-none-eabi-gcc GNU)
CMAKE_FORCE_CXX_COMPILER(arm-none-eabi-g++ GNU)

# extend linker invocations so that map files are created
SET( CMAKE_C_LINK_EXECUTABLE
     "<CMAKE_C_COMPILER> <FLAGS> <CMAKE_C_LINK_FLAGS> -Wl,-Map,<TARGET>.map <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
SET( CMAKE_CXX_LINK_EXECUTABLE
     "<CMAKE_CXX_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> -Wl,-Map,<TARGET>.map <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

# set the ARCHITECTURE for the framework
SET(ARCHITECTURE "ARCH_CORTEXM0"
  CACHE STRING "ARCH_X86, ARCH_X86_64, ARCH_CORTEXM0" FORCE)

# extend the compile flags with the needed parameters
SET(TOOLCHAIN_C_FLAGS      "-mcpu=${CPU} -mthumb -ffunction-sections -fdata-sections")
SET(TOOLCHAIN_CXX_FLAGS    "-mcpu=${CPU} -mthumb -ffunction-sections -fdata-sections")
SET(TOOLCHAIN_LINKER_FLAGS "-mcpu=${CPU} -mthumb -Wl,-gc-sections")
# SET(TOOLCHAIN_LINKER_FLAGS "-mcpu=${CPU} -mthumb -Wl,-static -Wl,-gc-sections -Wl,-Tsrc/arch/arm_cortex_m0_gcc/cm0.ld --specs=nano.specs")

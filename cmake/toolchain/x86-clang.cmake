# to compile with this toolchain call cmake like this
# e.g.: cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain/arm-none-eabi-gcc.cmake ..

SET(CMAKE_C_COMPILER clang)
SET(CMAKE_CXX_COMPILER clang++)

# extend the compile flags with the needed parameters
SET(TOOLCHAIN_C_FLAGS       "-ffunction-sections -fdata-sections -fsanitize=address")
SET(TOOLCHAIN_CXX_FLAGS     "-ffunction-sections -fdata-sections -fsanitize=address")
SET(TOOLCHAIN_LINKER_FLAGS  "-Wl,-gc-sections")

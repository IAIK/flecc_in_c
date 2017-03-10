find_package( StandardMathLibrary REQUIRED )

file(GLOB_RECURSE Sources "*.c")

add_library(flecc_in_c ${Sources})
target_link_libraries(flecc_in_c ${STANDARD_MATH_LIBRARY})
target_include_directories(flecc_in_c
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include>)

install_lib(TARGETS flecc_in_c
            EXPORT flecc_in_c
            DESTINATION "lib${LIBDIR_INSTALL_SUFFIX}")

export_and_install_config(flecc_in_c)

# register source files for reformating
add_file_to_format(${Sources})

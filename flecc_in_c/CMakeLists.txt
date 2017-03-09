# build the flecc_in_c library
#-----------------------------------------
file(GLOB_RECURSE Sources "*.c")

add_library(flecc_in_c ${Sources})
set_target_properties(flecc_in_c PROPERTIES SUFFIX "-${ARCHITECTURE}")
target_compile_definitions(flecc_in_c INTERFACE "ARCHITECTURE=${ARCHITECTURE}")
target_link_libraries(flecc_in_c ${STANDARD_MATH_LIBRARY})
target_include_directories(flecc_in_c PRIVATE "."
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
         $<INSTALL_INTERFACE:include>)

install_lib(TARGETS flecc_in_c EXPORT flecc_in_c DESTINATION "lib${LIBDIR_INSTALL_SUFFIX}")
install_lib(DIRECTORY "flecc_in_c/" DESTINATION "include/flecc_in_c"
                         FILES_MATCHING PATTERN "*.h")
export_and_install_config(flecc_in_c)

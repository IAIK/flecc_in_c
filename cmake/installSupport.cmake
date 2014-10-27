# This script provides install related targets and functions. This includes
# an simple uninstall target.
#
# Provided targets:
#   uninstall.............uninstalls files based on the install_manifest
#
# Provided macros/functions:
#   --

# configure file which is needed for uninstall target
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

# add uninstall target to the make file
ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake") 

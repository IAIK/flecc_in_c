# enable the ctest support
enable_testing()

# add check command which calls the test target
# additionally it depends on the test cases and builds them
IF(NOT TARGET suite)
  add_custom_target(suite)
ENDIF(NOT TARGET suite)

IF(NOT TARGET check)
  add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
                          WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                          COMMENT "Executing test suite.")
  add_dependencies(check "suite")
ENDIF(NOT TARGET check)

# copy the helper script into the build directory
configure_file("${CMAKE_SOURCE_DIR}/cmake/run_with_stdin_pipe.sh"
               "${CMAKE_BINARY_DIR}/run_with_stdin_pipe.sh" COPYONLY)

# adds a test with a file for stdin
macro(add_stdin_test name target stdinfile)
  add_test(NAME "${name}"
           COMMAND "${CMAKE_BINARY_DIR}/run_with_stdin_pipe.sh" $<TARGET_FILE:${target}> "${stdinfile}")
endmacro()

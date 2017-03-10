file(GLOB Sources "*.c")

add_executable(testrunner ${Sources})
target_link_libraries(testrunner flecc_in_c)
add_to_suite(testrunner)

# register source files for reformating
add_file_to_format(${Sources})

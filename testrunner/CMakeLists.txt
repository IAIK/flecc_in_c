file(GLOB TestrunnerSources "*.c")

add_executable(testrunner ${TestrunnerSources})
target_link_libraries(testrunner flecc_in_c)
add_to_suite(testrunner)

if(NOT TEST_COVERAGE)
  # test if the targets are findable from the build directory
  add_test(find-package-test ${CMAKE_CTEST_COMMAND}
    -C ${CMAKE_BUILD_TYPE}
    --build-and-test
    "${CMAKE_CURRENT_SOURCE_DIR}/find-package-test"
    "${CMAKE_CURRENT_BINARY_DIR}/find-package-test"
    --build-generator ${CMAKE_GENERATOR}
    --build-makeprogram ${CMAKE_MAKE_PROGRAM}
    --build-options "-Dflecc_in_c_DIR=${PROJECT_BINARY_DIR}"
                    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")

  # test if the targets are findable when add_subdirectory is used
  add_test(add-subdirectory-test ${CMAKE_CTEST_COMMAND}
    -C ${CMAKE_BUILD_TYPE}
    --build-and-test
    "${CMAKE_CURRENT_SOURCE_DIR}/add-subdirectory-test"
    "${CMAKE_CURRENT_BINARY_DIR}/add-subdirectory-test"
    --build-generator ${CMAKE_GENERATOR}
    --build-makeprogram ${CMAKE_MAKE_PROGRAM}
    --build-options "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
endif()

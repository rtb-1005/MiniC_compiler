add_executable(test_error_reporter test_error_reporter.c ${CMAKE_SOURCE_DIR}/src/error_reporter.c)
target_include_directories(test_error_reporter PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME error_reporter COMMAND test_error_reporter)

# Run minic on a valid file
add_test(NAME parse_valid COMMAND $<TARGET_FILE:minic> ${CMAKE_SOURCE_DIR}/input.minic)

# Run minic on an invalid file, expect failure
add_test(NAME parse_invalid COMMAND $<TARGET_FILE:minic> ${CMAKE_SOURCE_DIR}/tests/bad.minic)
set_tests_properties(parse_invalid PROPERTIES WILL_FAIL TRUE)

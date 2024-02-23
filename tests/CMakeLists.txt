include(CTest)

set(
  TEST_SOURCES
  test_stringswitch_creation.cpp
)

foreach(TEST_SOURCE ${TEST_SOURCES})
  string(REPLACE ".cpp" "" TEST_BINARY ${TEST_SOURCE})
  add_executable(${TEST_BINARY} ${TEST_SOURCE})
  target_link_libraries(${TEST_BINARY} PRIVATE stringswitch)
  add_test(NAME ${TEST_BINARY} COMMAND $<TARGET_FILE:${TEST_BINARY}>)
endforeach()

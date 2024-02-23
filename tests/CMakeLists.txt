add_executable(
  test_stringswitch_creation
  test_stringswitch_creation.cpp
)
target_link_libraries(
  test_stringswitch_creation
  PRIVATE
  stringswitch
)
add_test(test_stringswitch_construction COMMAND $<TARGET_FILE:test_stringswitch_creation>)

add_library(stringswitch INTERFACE)

target_include_directories(
  stringswitch
  INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_DIR}/include>
)

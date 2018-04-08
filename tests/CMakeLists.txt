set(TARGET_NAME hope-test)

add_executable(${TARGET_NAME} main.cpp)
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 14)
target_link_libraries(${TARGET_NAME} hope)
include_directories(${CMAKE_SOURCE_DIR}/lib/include)
if (UNIX)
  target_link_libraries(${TARGET_NAME} pthread)
endif()

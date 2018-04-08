set(TARGET_NAME hope-test)

add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} hope)
include_directories(${CMAKE_SOURCE_DIR}/lib/include)
if (UNIX)
  target_link_libraries(${TARGET_NAME} pthread)
endif()

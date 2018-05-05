set(TARGET_NAME hope-example)

add_executable(hello_world hello_world.cpp)
target_link_libraries(hello_world hope)
include_directories(${CMAKE_SOURCE_DIR}/lib/include)


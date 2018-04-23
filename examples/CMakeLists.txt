set(TARGET_NAME hope-example)

file(GLOB TARGET_SOURCES *.cpp)
add_executable(${TARGET_NAME} ${TARGET_SOURCES})
target_link_libraries(${TARGET_NAME} hope)
include_directories(${CMAKE_SOURCE_DIR}/lib/include)


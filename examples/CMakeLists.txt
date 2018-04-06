set(TARGET_NAME HopeExample)

file(GLOB TARGET_SOURCES *.cpp)
add_executable(${TARGET_NAME} ${TARGET_SOURCES})
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 11)
target_link_libraries(${TARGET_NAME} Hope)
include_directories(${CMAKE_SOURCE_DIR}/lib/include)


set(TARGET_NAME Hope)

file(GLOB_RECURSE TARGET_SOURCES *.cpp)

add_library(${TARGET_NAME} SHARED ${TARGET_SOURCES})
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 14)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/hope)

if (UNIX)
  target_link_libraries(${TARGET_NAME} pthread)
endif()

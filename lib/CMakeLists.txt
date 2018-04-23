set(TARGET_NAME hope)

set(TARGET_MAJOR 0)
set(TARGET_MINOR 1)
set(TARGET_PATCH 0)

file(GLOB_RECURSE TARGET_SOURCES *.cpp)
file(GLOB_RECURSE TARGET_HEADERS *.h)

add_library(${TARGET_NAME} SHARED ${TARGET_SOURCES} ${TARGET_HEADERS})
set_target_properties(${TARGET_NAME}
PROPERTIES
  SOVERSION "${TARGET_MAJOR}.${TARGET_MINOR}"
  VERSION "${TARGET_MAJOR}.${TARGET_MINOR}.${TARGET_PATCH}"
)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/hope)

if (UNIX)
  target_link_libraries(${TARGET_NAME} pthread)
endif()


# Install directive for header files
install(FILES
        ${TARGET_HEADERS}
        DESTINATION include/hope)

# Install directive for binaries
install(TARGETS ${TARGET_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        )

# Static version
add_library(${TARGET_NAME}Static STATIC ${TARGET_SOURCES} ${TARGET_HEADERS})

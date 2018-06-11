set(TARGET_NAME hope-example)

add_executable(hello_world hello_world.cpp)
target_link_libraries(hello_world hope)
include_directories(${CMAKE_SOURCE_DIR}/lib/include)
target_compile_definitions(hello_world PRIVATE -DHOPE_USE_SHARED)

add_executable(producer_consumer producer_consumer.cpp)
target_link_libraries(producer_consumer hope)
include_directories(${CMAKE_SOURCE_DIR}/lib/include)
target_compile_definitions(producer_consumer PRIVATE -DHOPE_USE_SHARED)

add_executable(threaded_producer_consumer threaded_producer_consumer.cpp)
target_link_libraries(threaded_producer_consumer hope)
include_directories(${CMAKE_SOURCE_DIR}/lib/include)
target_compile_definitions(threaded_producer_consumer PRIVATE -DHOPE_USE_SHARED)

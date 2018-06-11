enable_testing()

find_package(GTest)

if(${GTEST_FOUND})
    set(TARGET_NAME hope-test)
    add_executable(${TARGET_NAME}
        main.cpp
        test_application.cpp
        test_timer.cpp
        test_signal.cpp
        helpers/applicationtesthelper.h
        helpers/eventlooptesthelper.h
    )
    target_link_libraries(${TARGET_NAME} hopeStatic gtest_main)
    include_directories(${CMAKE_SOURCE_DIR}/lib/include)
    if (UNIX)
      target_link_libraries(${TARGET_NAME} pthread)
    endif()
    add_test(AllTestsInFoo ${TARGET_NAME})
endif()


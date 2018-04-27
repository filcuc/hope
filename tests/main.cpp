#include "hope/eventloop.h"
#include "hope/timer.h"
#include "hope/thread.h"

#include <gtest/gtest.h>

#include <thread>
#include <future>

using namespace hope;


int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

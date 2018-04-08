#include "hope/eventloop.h"
#include "hope/timer.h"
#include "hope/thread.h"

#include <gtest/gtest.h>

#include <thread>
#include <future>

using namespace hope;


TEST(EventLoopTest, ExecAndQuit) {
    int exit_code = 0;
    EventLoop* event_loop;
    std::thread t([&]{
        EventLoop ev;
        event_loop = &ev;
        exit_code = ev.exec();
    });
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ASSERT_TRUE(event_loop->is_running());
    event_loop->quit(5);
    ASSERT_TRUE(t.joinable());
    t.join();
    ASSERT_EQ(5, exit_code);
}

TEST(Object, NullEventLoopThreadVariableTest) {
    Object object;
    ASSERT_EQ(nullptr, object.event_loop());
    ASSERT_EQ(std::this_thread::get_id(), object.thread_id());
}

TEST(Object, EventLoopThreadVariableTest) {
    EventLoop event_loop;
    Object object;
    ASSERT_EQ(&event_loop, object.event_loop());
    ASSERT_EQ(std::this_thread::get_id(), object.thread_id());
}

TEST(Object, MoveToThread) {
    Thread thread;
    thread.start();
    Object object;
    ASSERT_EQ(nullptr, object.event_loop());
    ASSERT_EQ(std::this_thread::get_id(), object.thread_id());
    object.move_to_thread(&thread);
    ASSERT_EQ(thread.event_loop(), object.event_loop());
    ASSERT_EQ(thread.id(), object.thread_id());
}

TEST(Thread, StartQuitWait) {
    Thread thread;
    ASSERT_NE(std::this_thread::get_id(), thread.id());
    ASSERT_EQ(nullptr, thread.event_loop());
    thread.start();
    ASSERT_NE(nullptr, thread.event_loop());
    thread.quit();
    thread.wait();
}

TEST(TimerTest, SimpleUsage) {
    EventLoop event_loop;
    Timer timer;
    std::chrono::milliseconds duration(1000);
    timer.set_duration(duration);
    ASSERT_EQ(timer.duration(), duration);
    timer.triggered().connect([&] { event_loop.quit(); });
    timer.start();
    event_loop.exec();
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

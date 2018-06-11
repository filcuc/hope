/*
    Copyright (C) 2018 Filippo Cucchetto.
    Contact: https://github.com/filcuc/hope

    This file is part of the Hope library.

    The Hope library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    The Hope library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with the Hope library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "helpers/applicationtesthelper.h"
#include "helpers/eventlooptesthelper.h"

#include <gtest/gtest.h>

using namespace hope;
using namespace test;

class ApplicationFixture : public ::testing::Test {
protected:
    virtual void SetUp() {
    }

    std::mutex mutex;
    std::condition_variable cond;
};

TEST_F(ApplicationFixture, CreationTest) {
    Application app;
    ASSERT_FALSE(ApplicationTestHelper::event_loop(app).is_running());
}

TEST_F(ApplicationFixture, ExecTest) {
    std::unique_lock<std::mutex> lock(mutex);

    Application* app = nullptr;

    std::thread t([&] {
        mutex.lock();
        Application a;
        app = &a;
        cond.notify_one();
        mutex.unlock();
        a.exec();
    });

    cond.wait(lock, [&] { return app != nullptr; });
    std::this_thread::sleep_for(std::chrono::seconds(1));

    ASSERT_TRUE(ApplicationTestHelper::event_loop(*app).is_running());

    app->quit();
    t.join();
}

TEST_F(ApplicationFixture, QuitTest) {
    std::unique_lock<std::mutex> lock(mutex);

    Application* app = nullptr;
    int exit_code = -1;
    const int EXPECTED_EXIT_CODE = 40;

    std::thread t([&] {
        mutex.lock();
        Application a;
        app = &a;
        cond.notify_one();
        mutex.unlock();
        exit_code = a.exec();
    });

    cond.wait(lock, [&] { return app != nullptr; });
    std::this_thread::sleep_for(std::chrono::seconds(1));

    ASSERT_TRUE(ApplicationTestHelper::event_loop(*app).is_running());
    app->quit(EXPECTED_EXIT_CODE);
    t.join();
    ASSERT_EQ(exit_code, EXPECTED_EXIT_CODE);
}

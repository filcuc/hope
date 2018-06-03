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

#include <hope/timer.h>

#include <gtest/gtest.h>

using namespace hope;
using namespace test;

using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;
using Milliseconds = std::chrono::milliseconds;
using Seconds = std::chrono::seconds;

static TimePoint triggered_time;
static bool triggered = false;

class TestReceiver : public Object {
public:
    void on_triggered() {
        triggered = true;
        triggered_time = Clock::now();
    }
};

class TimerFixture : public ::testing::Test {
protected:
    virtual void SetUp() {
        triggered = false;
    }

    Application app;
};

TEST_F(TimerFixture, CreationTest) {
    Timer timer;
}

TEST_F(TimerFixture, DurationTest) {
    Timer timer;
    ASSERT_EQ(timer.duration(), Milliseconds(0));
}

TEST_F(TimerFixture, SetDurationTest) {
    Timer timer;
    timer.set_duration(Milliseconds(4000));
    ASSERT_EQ(timer.duration(), Milliseconds(4000));
}

TEST_F(TimerFixture, TestTriggerSignal) {
    TestReceiver receiver;
    Timer timer;
    timer.set_duration(Milliseconds(1000));
    timer.triggered().connect(&receiver, &TestReceiver::on_triggered);

    std::thread t([this] {
        std::this_thread::sleep_for(Milliseconds(1000));
        app.quit();
    });

    timer.start();
    TimePoint start_time = Clock::now();
    app.exec();
    t.join();

    ASSERT_EQ(true, triggered);
    auto duration = std::chrono::duration_cast<Milliseconds>(triggered_time - start_time);
    ASSERT_GE(duration, Milliseconds(1000));
    ASSERT_LT(duration, Milliseconds(2000));
}





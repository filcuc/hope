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

class TimerFixture : public ::testing::Test {
protected:
    virtual void SetUp() {
    }

    Application app;
};

TEST_F(TimerFixture, CreationTest) {
    Timer timer;
    ASSERT_EQ(timer.thread_id(), std::this_thread::get_id());
}

TEST_F(TimerFixture, DurationTest) {
    Timer timer;
    ASSERT_EQ(timer.duration(), std::chrono::milliseconds(0));
}

TEST_F(TimerFixture, SetDurationTest) {
    Timer timer;
    timer.set_duration(std::chrono::milliseconds(4000));
    ASSERT_EQ(timer.duration(), std::chrono::milliseconds(4000));
}





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

#include <hope/application.h>
#include <hope/object.h>
#include <hope/objectinvoker.h>
#include <hope/signal.h>
#include <hope/thread.h>

#include <gtest/gtest.h>

using namespace hope;

namespace {

class TestReceiver : public Object {
public:
    TestReceiver()
        : num_calls(0)
    {}

    void test_receiver() {
        ++num_calls;
    }

    void test_receiver2(int value) {
        ++num_calls;
        last_value = value;
    }

    int num_calls = 0;
    int last_value = 0;
};


class ObjectInvokerFixture : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

TEST_F(ObjectInvokerFixture, TestSimpleUsage) {
    Application app;
    TestReceiver receiver;
    hope::ObjectInvoker::invoke_auto(&receiver, &TestReceiver::test_receiver);
    ASSERT_EQ(1, receiver.num_calls);
}

}


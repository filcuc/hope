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
#include <hope/signal.h>
#include <hope/thread.h>

#include <gtest/gtest.h>

using namespace hope;

class TestSender : public Object {
public:
    Signal<>& test_signal() {
        return m_test_signal;
    }

private:
    Signal<> m_test_signal;
};

class TestReceiver : public Object {
public:
    void test_receiver() {
        ++num_calls;
    }

    int num_calls = 0;
};

class SignalFixture : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

TEST_F(SignalFixture, TestSignalInvokationWithoutConnection) {
    Application app;
    TestSender sender;
    TestReceiver receiver;
    sender.test_signal().emit();
    ASSERT_EQ(0, receiver.num_calls);
}

TEST_F(SignalFixture, TestDirectInvokation) {
    Application app;
    TestSender sender;
    TestReceiver receiver;
    sender.test_signal().connect(&receiver, &TestReceiver::test_receiver);
    sender.test_signal().emit();
    ASSERT_EQ(1, receiver.num_calls);
}

TEST_F(SignalFixture, TestQueuedInvokation) {
    Application app;
    TestSender sender;
    auto receiver = new TestReceiver();
    sender.test_signal().connect(receiver, &TestReceiver::test_receiver);
    Thread thread;
    thread.start();
    thread.move_to_thread(std::unique_ptr<TestReceiver>(receiver));
    sender.test_signal().emit();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ASSERT_EQ(1, receiver->num_calls);
}

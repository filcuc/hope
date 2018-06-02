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

static int num_calls = 0;
static int last_value = 0;

class TestSender : public Object {
public:
    Signal<>& test_signal() {
        return m_test_signal;
    }

    Signal<int>& test_signal2() {
        return m_test_signal2;
    }

private:
    Signal<> m_test_signal;
    Signal<int> m_test_signal2;
};

class TestReceiver : public Object {
public:
    void test_receiver() {
        ++num_calls;
    }

    void test_receiver2(int value) {
        ++num_calls;
        last_value = value;
    }
};

class SignalFixture : public ::testing::Test {
protected:
    void SetUp() override {
        num_calls = 0;
        last_value = 0;
    }
};

TEST_F(SignalFixture, TestSignalInvokationWithoutConnection) {
    Application app;
    TestSender sender;
    TestReceiver receiver;
    sender.test_signal().emit();
    ASSERT_EQ(0, num_calls);
}

TEST_F(SignalFixture, TestDirectInvokation) {
    Application app;
    TestSender sender;
    TestReceiver receiver;
    sender.test_signal().connect(&receiver, &TestReceiver::test_receiver);
    sender.test_signal().emit();
    ASSERT_EQ(1, num_calls);
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
    ASSERT_EQ(1, num_calls);
}

TEST_F(SignalFixture, TestDirectInvokationWithIntSignal) {
    Application app;
    TestSender sender;
    TestReceiver receiver;
    sender.test_signal2().connect(&receiver, &TestReceiver::test_receiver2);
    sender.test_signal2().emit(30);
    ASSERT_EQ(1, num_calls);
    ASSERT_EQ(30, last_value);
}

TEST_F(SignalFixture, TestQueuedInvokationWithIntSignal) {
    Application app;
    TestSender sender;
    auto receiver = new TestReceiver();
    sender.test_signal2().connect(receiver, &TestReceiver::test_receiver2);
    Thread thread;
    thread.start();
    thread.move_to_thread(std::unique_ptr<TestReceiver>(receiver));
    sender.test_signal2().emit(30);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ASSERT_EQ(1, num_calls);
    ASSERT_EQ(30, last_value);
}

TEST_F(SignalFixture, TestDisconnectionWithConnection) {
    Application app;
    TestSender sender;
    TestReceiver receiver;
    auto connection = sender.test_signal().connect(&receiver, &TestReceiver::test_receiver);
    sender.test_signal().emit();
    ASSERT_EQ(1, num_calls);
    sender.test_signal().disconnect(connection);
    sender.test_signal().emit();
    ASSERT_EQ(1, num_calls);
}

TEST_F(SignalFixture, TestDisconnectionWithFunctionSignature) {
    Application app;
    TestSender sender;
    TestReceiver receiver;
    sender.test_signal().connect(&receiver, &TestReceiver::test_receiver);
    sender.test_signal().emit();
    ASSERT_EQ(1, num_calls);
    sender.test_signal().disconnect(&receiver, &TestReceiver::test_receiver);
    sender.test_signal().emit();
    ASSERT_EQ(1, num_calls);
}

TEST_F(SignalFixture, TestAutoDisconnectionOnDestroy) {
    Application app;
    TestSender sender;
    std::unique_ptr<TestReceiver> receiver(new TestReceiver());
    sender.test_signal().connect(receiver.get(), &TestReceiver::test_receiver);
    sender.test_signal().emit();
    ASSERT_EQ(1, num_calls);
    receiver.reset();
    sender.test_signal().emit();
    ASSERT_EQ(1, num_calls);
}

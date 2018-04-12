#include "hope/application.h"
#include "hope/eventloop.h"
#include "hope/timer.h"
#include "hope/thread.h"

#include <atomic>

using namespace hope;

class Consumer : public Object {
public:
    void consume(int k, int j) {
        std::cout << "Consuming with two args" << k << " " << j << std::endl;
    }

    void consume_2(std::unique_ptr<int> k) {
        std::cout << "Consuming with movable arg" << *k << std::endl;
    }

    void consume_3() {
        std::cout << "Consuming without any arg" << std::endl;
    }
};

class Producer : public Object {
public:
    Signal<int, int>& produce_signal() {
        return m_signal;
    }

    Signal<std::unique_ptr<int>>& produce_signal_2() {
        return m_signal_2;
    }

    Signal<void>& produce_signal_3() {
        return m_signal_3;
    }

    void produce() {
        m_signal.emit(10, 40);
        m_signal_2.emit(std::unique_ptr<int>(new int(320)));
        m_signal_3.emit();
    }

private:
    Signal<int, int> m_signal;
    Signal<std::unique_ptr<int>> m_signal_2;
    Signal<void> m_signal_3;
};

int main()
{
    Application app;

    Consumer printer;

    Producer producer;
    producer.produce_signal().connect(&printer, &Consumer::consume);
    producer.produce_signal_2().connect(&printer, &Consumer::consume_2);
    producer.produce_signal_3().connect(&printer, &Consumer::consume_3);
    producer.produce();

    return app.exec();
}

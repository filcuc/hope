#include "hope/eventloop.h"
#include "hope/timer.h"
#include "hope/thread.h"

#include <atomic>

using namespace hope;

class Consumer : public Object {
public:
    void consume(int k, int j) {
        std::cout << "Argh" << k << " " << j << std::endl;
    }

    void consume_2(std::unique_ptr<int> k) {
        std::cout << "Argh" << *k << std::endl;
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

    void produce() {
        m_signal.emit(10, 40);
        m_signal_2.emit(std::make_unique<int>(320));
    }

private:
    Signal<int, int> m_signal;
    Signal<std::unique_ptr<int>> m_signal_2;
};

int main()
{
    EventLoop e;
    Consumer printer;
    Thread tasf;
    tasf.start();
    printer.move_to_thread(&tasf);

    Producer producer;
    producer.produce_signal().connect(&printer, &Consumer::consume);
    producer.produce_signal_2().connect(&printer, &Consumer::consume_2);
    producer.produce();
    e.exec();
    tasf.wait();
}

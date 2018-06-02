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
#include <hope/thread.h>
#include <hope/timer.h>

using namespace hope;

using Product = std::string;

struct Producer : public Object {
    Producer()
    {
        m_timer.set_duration(std::chrono::seconds(1));
        m_timer.triggered().connect(this, &Producer::on_triggered);
    }

    Signal<>& finished() {
        return m_finished_signal;
    }

    Signal<Product>& product_available() {
        return m_product_available_signal;
    }

    void produce() {
        m_timer.start();
    }

private:
    void on_triggered() {
        if (num_products != 5) {
            auto product = "Product " + std::to_string(num_products++);
            std::cout << "Producing product " << product << " from thread " << std::this_thread::get_id() << std::endl;
            m_product_available_signal.emit(std::move(product));
            m_timer.start();
        } else {
            m_finished_signal.emit();
        }
    }

    Timer m_timer;
    Signal<Product> m_product_available_signal;
    Signal<> m_finished_signal;
    int num_products = 0;
};

struct Consumer : public Object {
    void consume(Product product) {
        std::cout << "Consuming " << product << " from thread " << std::this_thread::get_id() << std::endl;
    }
};

int main(int argc, char* argv[])
{
    Application app;

    std::unique_ptr<Consumer> consumer (new Consumer());

    Producer producer;
    producer.product_available().connect(consumer.get(), &Consumer::consume);
    producer.finished().connect(&app, &Application::quit);

    Thread consumer_thread;
    consumer_thread.start();
    consumer_thread.move_to_thread(std::move(consumer));

    producer.produce();

    return app.exec();
}

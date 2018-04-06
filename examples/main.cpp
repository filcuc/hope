#include "hope/eventloop.h"
#include "hope/timer.h"

using namespace hope;

int main()
{
    EventLoop e;
    Timer timer;
    timer.set_duration(std::chrono::milliseconds(3000));
    timer.set_on_triggered([]{
        std::cout << "Hello world" << std::endl;
    });
    timer.start();
    return e.exec();
}

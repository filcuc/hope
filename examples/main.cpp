#include "hope/eventloop.h"
#include "hope/timer.h"

using namespace hope;

int main()
{
    EventLoop e;
    Timer timer;
    timer.set_duration(std::chrono::milliseconds(3000));
    timer.triggered().connect([]{
        std::cout << "Hello World" << std::endl;
    });
    timer.start();
    return e.exec();
}

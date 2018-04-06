#include "hope/eventloop.h"
#include "hope/timer.h"

int main()
{
    EventLoop e;
    return e.exec();
}

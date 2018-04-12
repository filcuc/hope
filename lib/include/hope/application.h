#pragma once

#include "eventloop.h"

namespace hope {

class Application {
public:
    Application();

    void quit(int exit_code);

    int exec();

private:
    EventLoop m_event_loop;
};

}

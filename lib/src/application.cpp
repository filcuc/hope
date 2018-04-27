#include "hope/application.h"

#include "hope/private/threaddata.h"

namespace hope {

Application::Application()
{
}

void Application::quit(int exit_code)
{
   m_event_loop.quit(exit_code);
}

int Application::exec()
{
    return m_event_loop.exec();
}

}

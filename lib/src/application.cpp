#include "hope/application.h"

#include "hope/threaddata.h"

namespace hope {

Application::Application()
{
    ThreadDataRegistry::get_instance().current_thread_data()->set_event_loop(&m_event_loop);
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

#include "hope/application.h"

#include "hope/threaddata.h"

namespace hope {

Application::Application()
{
    ThreadData data(std::this_thread::get_id(), &m_event_loop);
    ThreadDataRegistry::get_instance().set_thread_data(std::move(data));
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

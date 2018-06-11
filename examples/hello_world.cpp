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
#include <hope/timer.h>

using namespace hope;

class HelloWorldPrinter : public Object {
public:
    void say_hello() {
        std::cout << "Hello World" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    Application app;

    HelloWorldPrinter printer;

    Timer timer;
    timer.set_duration(std::chrono::seconds(1));
    timer.triggered().connect(&printer, &HelloWorldPrinter::say_hello);
    timer.triggered().connect(&app, &Application::quit);
    timer.start();

    return app.exec();
}

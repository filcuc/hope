# Hope 
## A simple pure C++11 event loop with signals and slots 
[![Build Status](https://travis-ci.org/filcuc/hope.svg?branch=master)](https://travis-ci.org/filcuc/hope)
[![Build status](https://ci.appveyor.com/api/projects/status/b2spyqd13op654j3/branch/master?svg=true)](https://ci.appveyor.com/project/filcuc/hope/branch/master)
[![codecov](https://codecov.io/gh/filcuc/hope/branch/master/graph/badge.svg)](https://codecov.io/gh/filcuc/hope)


### Notes
This project doesn't aim to become a general purpose library.
It was done for fun and scratching my own itch.
That said it can be useful for simple command line projects

### How it looks?
```C++
#include <hope/application.h>
#include <hope/timer.h>

using namespace hope;

class HelloWorldPrinter : public Object {
public:
    void say_hello() {
        std::cout << "Hello World" << std::endl;
    }
};

int main(int argc, char* argv[])
{
    Application app;

    HelloWorldPrinter printer;

    Timer timer;
    timer.set_duration(std::chrono::seconds(1));
    timer.triggered().connect(&printer, &HelloWorldPrinter::say_hello);
    timer.triggered().connect(&app, &Application::quit);
    timer.start();

    return app.exec();
}

```

### Building
The project require a working C++11 compiler
```Shell
mkdir build
cd build 
cmake ..
cmake --build .
```

### Installing
Follow the build steps and simply execute
```Shell
sudo make install
```


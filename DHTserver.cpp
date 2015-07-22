
#include <exception>
#include <iostream>
// #include "myclient.h"
#include "myserver.h"

int main(int argc, char const* argv[])
{
    try {
        Myserver s("5321");
        // c.sendMessage("GET / HTTP/1.1\r\nHost: www.google.com\r\nConnection: close\r\n\r\n");
        s.createServer();
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    } catch (const char* c) {
        std::cout << c << std::endl;
    } catch (...) {
        // code
    }
    return 0;
}

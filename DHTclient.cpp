
#include <exception>
#include <iostream>
#include "myclient.h"

int main(int argc, char const* argv[])
{
    try {
        Myclient c("localhost", "5321");
        // c.sendMessage("GET / HTTP/1.1\r\nHost: www.google.com\r\nConnection: close\r\n\r\n");
        // std::cout << c.getIPdetails("www.google.com") << std::endl;
        c.createClient();
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    } catch (const char* c) {
        std::cout << c << std::endl;
    } catch (...) {
        // code
    }
    return 0;
}

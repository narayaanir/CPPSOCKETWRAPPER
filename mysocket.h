#ifndef MYSOCKET_H
#define MYSOCKET_H

#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

const unsigned int BACKLOG = 10;

class Mysocket {
private:
    std::string host;
    std::string port;
    struct addrinfo hints;

protected:
    struct addrinfo* result;
    int sockfd;
    // get sockaddr, IPv4 or IPv6:
    void* get_in_addr(struct sockaddr* sa)
    {
        if (sa->sa_family == AF_INET) {
            return &(((struct sockaddr_in*)sa)->sin_addr);
        }
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }

    // Gather details of IP details for the host
    void setIPdetails() {
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        if (getaddrinfo(host.c_str(), port.c_str(), &hints, &result) != 0)
            throw "ERROR: Unable to get server details.";
    }

    // Create socket and return code
    int& createSocket(struct addrinfo* a) {
        sockfd = socket(a->ai_family, a->ai_socktype, a->ai_protocol);
        return sockfd;
    }
public:
    // Constructor
    Mysocket (std::string p, std::string h="localhost") : host(h), port(p)
    {
        if (port!="http" && ( atoi(port.c_str()) < 1024 || atoi(port.c_str()) > 65535)  )
            throw "ERROR: Invalid port.";
    }

    // Display IP details of the host being connected to
    std::string getIPdetails(std::string h="localhost")
    {
        host = h; setIPdetails();
        std::string details;
        for (auto a = result; a; a=a->ai_next) {
            void* addr; char ipstr[INET6_ADDRSTRLEN];
            if (a->ai_family == AF_INET) {
                struct sockaddr_in* ip= (struct sockaddr_in*) a->ai_addr;
                addr = &(ip->sin_addr);
            } else {
                struct sockaddr_in6* ip= (struct sockaddr_in6*) a->ai_addr;
                addr = &(ip->sin6_addr);
            }
            inet_ntop(a->ai_family, addr, ipstr, sizeof ipstr);
            details += "\n" + std::string(ipstr);
        }
        details.replace(0, 1, "");
        return details;
    }

    // Copy Constructor
    Mysocket (const Mysocket& mc)  = delete;

    // Move Constructor
    Mysocket (Mysocket&& mc)  = delete;

    // Operator overloading
    Mysocket& operator=(Mysocket cn) = delete;

    // Destructor
    virtual ~Mysocket ()
    {
        if (result)
            freeaddrinfo(result);
    }

};

#endif /*MYSOCKET_H*/

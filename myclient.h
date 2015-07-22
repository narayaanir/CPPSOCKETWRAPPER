#ifndef MYCLIENT_H
#define MYCLIENT_H

#include <sys/wait.h>
#include <errno.h>
#include <iostream>
#include "mysocket.h"

const int MAXDATASIZE = 1024;

class Myclient : public Mysocket {
private:

public:
    // Constructor
    Myclient (std::string h="localhost", std::string p="8080") : Mysocket(p, h)
    {
        setIPdetails();
    }

    void createClient()
    {
        // loop through all the results and connect to the first we can
        auto a = result;
        for(; a; a = a->ai_next) {
            if (createSocket(a) == -1) {
                perror("client: socket");
                continue;
            }

            if (connect(sockfd, a->ai_addr, a->ai_addrlen) == -1) {
                close(sockfd);
                perror("client: connect");
                continue;
            }

            break;
        }

        if (!a) {
            fprintf(stderr, "client: failed to connect\n");
            return;
        }

        char s[INET6_ADDRSTRLEN];
        inet_ntop(a->ai_family, get_in_addr((struct sockaddr*) a->ai_addr), s, sizeof s);
        std::cout << "Connecting to " << s << "..." << std::endl;

        int numbytes;
        char buf[MAXDATASIZE];
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }

        buf[numbytes] = '\0';
        std::cout << buf << std::endl;
        close(sockfd);
    }

    // Copy Constructor
    Myclient (const Myclient& mc) = delete;

    // Move Constructor
    Myclient (Myclient&& mc) = delete;

    // Operator overloading
    Myclient& operator=(Myclient cn) = delete;

    // Destructor
    virtual ~Myclient () {}
};

#endif /*MYCLIENT_H*/

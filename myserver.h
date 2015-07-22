#ifndef MYSERVER_H
#define MYSERVER_H

#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include "mysocket.h"

class Myserver : public Mysocket {
private:
    std::string message;
    struct sigaction sa;
    static void sigchld_handler(int s)
    {
        while(waitpid(-1, NULL, WNOHANG) > 0);
    }

    /******************************************************************************
    * Name          : findClientDetails

    * Description   : finds all clients connected to socket provided

    * Parameters    :
        int&    r   : port number of connected socket

    ******************************************************************************/
    void findClientDetails(int& r)
    {
        socklen_t len;
        struct sockaddr_storage addr;
        char ipstr[INET6_ADDRSTRLEN];
        int c_port;

        len = sizeof addr;
        if (getpeername(r, (struct sockaddr*)&addr, &len) == -1)
            throw "ERROR: Unable to get client details.";

        if (addr.ss_family == AF_INET) {
            struct sockaddr_in *s = (struct sockaddr_in *)&addr;
            c_port = ntohs(s->sin_port);
            inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
        } else {
            struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
            c_port = ntohs(s->sin6_port);
            inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
        }
        std::cout << "Connected to client with IP " << ipstr << " on " << c_port << std::endl;
    }

public:
    // Constructor
    Myserver (std::string p) : Mysocket(p)
    {
        sa.sa_handler = sigchld_handler; // reap all dead processes
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        if (sigaction(SIGCHLD, &sa, NULL) == -1) {
            perror("sigaction");
            exit(1);
        }
        message = "Server: Hi!!! I am alive!";
        setIPdetails();
    }

    /******************************************************************************
    * Name          : createServer

    * Description   : creates the server process to listen to incoming TCP messages

    * Parameters    : None

    ******************************************************************************/
    void createServer()
    {
        struct sockaddr_storage their_addr; // connector's address information
        socklen_t sin_size;
        auto a=result;
        for (; a; a=a->ai_next) {
            if (createSocket(a) == -1)
                throw "ERROR: Creating socket.";
            int yes=1;
            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
                perror("setsockopt");
                exit(1);
            }

            if (bind(sockfd, a->ai_addr, a->ai_addrlen) == -1) {
                close(sockfd);
                perror("server: bind");
                continue;
            }
            break;
        }

        if (!a) {
            fprintf(stderr, "server: failed to connect\n");
            return;
        }

        if (listen(sockfd, BACKLOG) == -1) {
            perror("listen");
            exit(1);
        }


        char s[INET6_ADDRSTRLEN];
        int new_fd; // listen on sock_fd, new connection on new_fd
        while(true)
        { // main accept() loop
            sin_size = sizeof their_addr;
            new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
            if (new_fd == -1) {
                perror("accept");
                continue;
            }
            inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
            if (!fork()) { // this is the child process
                close(sockfd); // child doesn't need the listener
                findClientDetails(new_fd);
                if (send(new_fd, message.c_str(), message.length(), 0) == -1)
                    perror("send");
                close(new_fd);
                exit(0);
            }
            close(new_fd); // parent doesn't need this
        }
    }

    // Copy Constructor
    Myserver (const Myserver& mc)  = delete;

    // Move Constructor
    Myserver (Myserver&& mc)  = delete;

    // Operator overloading
    Myserver& operator=(Myserver cn)  = delete;

    // Destructor
    virtual ~Myserver () {}
};

#endif /*MYSERVER_H*/

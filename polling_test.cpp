#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <cstdio>
#include <sys/socket.h> // socket call, listen call
#include <arpa/inet.h> // inet_addr call, htons call, bind call, accept call
#include <sys/select.h> // select call
#include <vector>
#include <algorithm>
#include <iostream>

#define BUFFER_SIZE 100

typedef struct sockaddr   sa_t; /* generic sockaddr struct, needs to be casted to appropiate sockaddr family */

const char *send_msg = "HTTP/1.1 200 OK\r\nServer: webserv 0.0.1\r\nContent-Length: 28\r\n\r\n<title>si</title><p>hola</p>\n\r\n\r";

template< class Cont>
void p(Cont c) {
    std::cout << "[ ";
    for(typename Cont::iterator it = c.begin(); it != c.end(); it++) {
        std::cout << *it << " ";
    }
    std::cout << "]\n";
}

int main(void) {
    int passv_socket;

    const char* addr = "127.0.0.1";
    struct in_addr sin_addr = { .s_addr = inet_addr(addr) }; /* address of listening socket */
    struct sockaddr_in s_addr = {
        .sin_family = AF_INET,  /* internet v4 protocol */
        .sin_port = htons(8081), /* listening port (htons == host to short network byte) */
        .sin_addr = sin_addr,
        .sin_zero = { 0 }
    };

    /*        fd = socket(domain , TCP       , only one protocol for TCP type) */
    passv_socket = socket(PF_INET, SOCK_STREAM, 0); /* returns socket file descriptor */
    if (passv_socket == -1) {
        perror("socket() ");
        return EXIT_FAILURE;
    }
    if (bind(passv_socket, reinterpret_cast<sa_t *>(&s_addr), sizeof(s_addr)) == -1) {
        perror("bind() ");
        return EXIT_FAILURE;
    }
    if (listen(passv_socket, FD_SETSIZE) == -1) {
        perror("listen() ");
        return EXIT_FAILURE;
    }

    fd_set readfds;
    fd_set writefds;
    std::vector<int> ready_to_read;
    std::vector<int> ready_to_write;

    ready_to_read.push_back(passv_socket); /* passive socket always in read fd vector */
    while (1) { /* server loop */
        /* clean fd sets */
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);

        /* initialize fd sets with fd vectors */
        for (std::vector<int>::iterator it = ready_to_read.begin(); it != ready_to_read.end(); it++) {
            FD_SET(*it, &readfds);
        }
        for (std::vector<int>::iterator it = ready_to_write.begin(); it != ready_to_write.end(); it++) {
            FD_SET(*it, &writefds);
        }

        /* nfds = number of biggest fd + 1 */
        std::vector<int>::iterator fd_max_write = std::max_element(ready_to_write.begin(), ready_to_write.end());
        std::vector<int>::iterator fd_max_read = std::max_element(ready_to_read.begin(), ready_to_read.end());
        int nfds = std::max(fd_max_write != ready_to_write.end() ? *fd_max_write : 0, *fd_max_read) + 1;
        std::cout << "< nfds : " << nfds << " >\n";
        p(ready_to_read);
        p(ready_to_write);
        /* polling: returns if a socket is ready to read or write */
        std::cout << "[SERVER] polling...\n";
        if (select(nfds, &readfds, &writefds, NULL, NULL) == -1) {
            perror("select() ");
            return EXIT_FAILURE;
        }

        /* manual checking of ready to read connections with FD_ISSET macro */
        std::cout << "[SERVER] reading...\n";
        for (std::vector<int>::size_type i = 0; i < ready_to_read.size(); i++) {
            if (FD_ISSET(ready_to_read[i], &readfds)) {
                if (ready_to_read[i] == passv_socket) {
                    struct sockaddr_in  conn_addr;
                    socklen_t           conn_addr_len = sizeof(conn_addr);

                    int conn_fd = accept(ready_to_read[i], reinterpret_cast<sa_t *>(&conn_addr), &conn_addr_len);
                    if (conn_fd == -1) {
                        perror("accept() ");
                    }
                    std::cout << "< new connection: addr, port > = < " 
                              << inet_ntoa(conn_addr.sin_addr) << ", " 
                              << ntohs(conn_addr.sin_port) << " >\n";
                    ready_to_read.push_back(conn_fd);
                } else {
                    char buffer[BUFFER_SIZE];
                    int  read_ret;

                    memset(buffer, 0, BUFFER_SIZE);
                    while ((read_ret = read(ready_to_read[i], buffer, BUFFER_SIZE)) == BUFFER_SIZE) {
                        std::cout << buffer;
                        memset(buffer, 0, BUFFER_SIZE);                        
                    }
                    if (read_ret == 0) {
                        std::cout << "< connection " << ready_to_read[i] << " closed >\n";
                        close(ready_to_read[i]);
                    } else {
                        std::cout << buffer;
                        ready_to_write.push_back(ready_to_read[i]); /* now socket is expected to be written into */
                    }
                    ready_to_read.erase(ready_to_read.begin() + i); /* remove socket from read vector */
                }
            }
        }

        /* manual checking of ready to write connections with FD_ISSET macro */
        std::cout << "[SERVER] writing...\n";
        for (std::vector<int>::size_type i = 0; i < ready_to_write.size(); i++) {
            if (FD_ISSET(ready_to_write[i], &writefds)) {
                write(ready_to_write[i], send_msg, 94);
                ready_to_read.push_back(ready_to_write[i]);
                ready_to_write.erase(ready_to_write.begin() + i);
            }
        }
    }
}
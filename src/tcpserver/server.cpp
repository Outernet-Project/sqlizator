// Copyright 2015, Outernet Inc.
// Some rights reserved.
// This software is free software licensed under the terms of GPLv3. See COPYING
// file that comes with the source code, or http://www.gnu.org/licenses/gpl.txt.
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "tcpserver/exceptions.h"
#include "tcpserver/server.h"

namespace tcpserver {

Server::Server(const std::string& port): socket_(port),
                                         epoll_() {}

Server::~Server() {
    // TODO: close all open connections
}

void Server::start() {
    try {
        socket_.bind();
        socket_.listen();
    } catch (socket_error& e) {
        throw server_error(e.what());
    }
    try {
        epoll_.add(socket_.fd(), std::bind(&Server::accept_connection,
                                           this,
                                           std::placeholders::_1));
    } catch (epoll_error& e) {
        // TODO: log error
        throw server_error(e.what());
    }
    while (true) {
        try {
            epoll_.wait();
        } catch (epoll_error& e) {
            throw server_error(e.what());
        }
    }
}

void Server::accept_connection(int fd) {
    while (true) {
        std::unique_ptr<ClientSocket> p_clientsocket(new ClientSocket(fd));
        int in_fd;
        try {
            in_fd = p_clientsocket->accept();
        } catch (socket_error& e) {
            // TODO: log error
            continue;
        }
        if (in_fd == -1)
            break;

        clients_.insert(std::make_pair(in_fd, std::move(p_clientsocket)));
        try {
            epoll_.add(in_fd, std::bind(&Server::receive_data,
                                        this,
                                        std::placeholders::_1));
        } catch (epoll_error& e) {
            // TODO: log error
            clients_.erase(in_fd);
        }
    }
}

void Server::receive_data(int fd) {
    ClientSocket* p_clientsocket;
    try {
        p_clientsocket = clients_.at(fd).get();
    } catch (std::out_of_range& e) {
        // TODO: log invalid client
        return;
    }
    // socket found, read incoming data into input buffer
    byte_vec input;
    try {
        p_clientsocket->recv(&input);
    } catch (socket_error& e) {
        // TODO: log error
        // deleting the object will close the socket which will automatically
        // make epoll stop monitoring it as well
        clients_.erase(fd);
        return;
    } catch (connection_closed& e) {
        // delete socket, but still process the received data after
        clients_.erase(fd);
        return;
    }
    // process freshly read incoming data and write response into output buffer
    byte_vec output;
    handle(input, &output);
    // send response from output buffer back to client
    try {
        p_clientsocket->send(output);
    } catch (socket_error& e) {
        // TODO: log error
        clients_.erase(fd);
    }
}

}  // namespace tcpserver

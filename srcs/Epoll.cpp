/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/20 19:41:13 by vcereced          #+#    #+#             */
/*   Updated: 2024/09/27 18:33:03 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Master.hpp"

#include <stdio.h>
#include <stdlib.h>

int Master::setSockets(std::set<int> &ports)
{
	std::set<int>::iterator it = ports.begin();
	for (; it != ports.end(); ++it)
	{
		int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (serverSocket < 0)
			ft_error("Error creating socket");

		// Set socket to non-blocking
		fcntl(serverSocket, F_SETFL, O_NONBLOCK);

		struct sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(*it);
		serverAddr.sin_addr.s_addr = INADDR_ANY;

		if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
			ft_error("Error binding socket");

		if (listen(serverSocket, 10) < 0)
			ft_error("Error listening on socket");

		_ListenSockets.push_back(serverSocket);
		std::cout << GREEN << "Socket " << serverSocket << " listening on port " << *it << RESET << std::endl;
	}
	return (0);
}

/**
 * @brief Creates the epoll set and adds the server sockets to it.
 * epoll_create1(0) creates a new epoll instance.
 * @return int 
 */
int Master::setEvents()
{
    _epoll_fd = epoll_create1(0);
    if (_epoll_fd == -1)
        ft_error("Error creating epoll");

    std::vector<int>::iterator it = _ListenSockets.begin();
    for (; it != _ListenSockets.end(); it++)
    {
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLOUT;
        ev.data.fd = *it;
        if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, *it, &ev) == -1)
            ft_error("Error adding socket to epoll");
    }
    return (0);
}

/**
 * @brief Accepts a connection on a server socket and adds the client socket to the epoll set.
 * Creates a Client object and adds it to the _clientsMap.
 * 
 * @param socketToAccept 
 * @return int 
 */
int Master::clientAccept(int socketToAccept)
{
    Connection  client;
	sockaddr_in clientAddr;
    socklen_t   clientAddrSize = sizeof(clientAddr);
    int         clientSocket;
    
    clientSocket = accept(socketToAccept, (struct sockaddr *)&clientAddr, &clientAddrSize);
    if (clientSocket < 0)
       return(ft_error("Error accepting connection"));
    std::cout << GREEN << "Client socket: " << clientSocket << " accepted connection on listen socket " << socketToAccept << " from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << RESET << std::endl;
    // Set client socket to non-blocking
    fcntl(clientSocket, F_SETFL, O_NONBLOCK);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = clientSocket;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, clientSocket, &ev) == -1)
        return(ft_error("Error adding client socket to epoll"));
    
    client.setClientData(clientSocket, clientAddr, clientAddrSize, ev,  this->getclientMaxBodySize());
    _clientsMap[clientSocket] = client;
    return (0);
}

void Master::deleteConnection(int SocketAccepted)
{
    epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, SocketAccepted, NULL);
    close(SocketAccepted);
    _clientsMap.erase(SocketAccepted);
}

void Master::manageConnections(struct epoll_event *events, int nev)
{
    //std::string errPagePath;
    int         socketToAccept;
    bool        toAccept;
    bool        toManage;
    
    for (int i = 0; i < nev; ++i)
    {
        socketToAccept = events[i].data.fd;
        toAccept = std::find(_ListenSockets.begin(), _ListenSockets.end(), socketToAccept) != _ListenSockets.end();
        toManage = _clientsMap.find(socketToAccept) != _clientsMap.end();
        
        if (toAccept)
        {
            if (this->clientAccept(socketToAccept) < 0)
                ft_error("Error accepting client");
        }
        else if (toManage)
        { 
            std::cout << "Client socket: " << socketToAccept << " being managed..." << std::endl;
            try {
                this->manageConnection(_clientsMap[socketToAccept]);
            } catch (const ServerException &e) {
                
                std::cerr << RED << "ServerException: startEventLoop: " << e.what() << " -> " << e.getCode() << RESET << std::endl;
                this->manageError(e.getCode(), socketToAccept);
                // errPagePath = this->getErrPages()[e.getCode()];
                // _clientsMap[socketToAccept].setStatusCode(e.getCode());
                // _clientsMap[socketToAccept].serveErrorPage(errPagePath);
                // this->deleteConnection(socketToAccept);
                
            } catch (const std::exception &e) {
                
                std::cerr << RED << "ServerException: startEventLoop: " + std::string(e.what()) << RESET << std::endl;
                this->manageError(INTERNAL_SERVER_ERROR, socketToAccept);
                // errPagePath = this->getErrPages()[INTERNAL_SERVER_ERROR];
                // _clientsMap[socketToAccept].setStatusCode(INTERNAL_SERVER_ERROR);
                // _clientsMap[socketToAccept].serveErrorPage(errPagePath);
                // this->deleteConnection(socketToAccept);

            }
            if (_clientsMap[socketToAccept].getKeepAlive() == false)
            {
                std::cout << "Client socket: " << socketToAccept << " being disconected and deleted..." << std::endl;
                this->deleteConnection(socketToAccept); //TODO: Check if this is the right place to delete the connection
            }
            // std::cout << "Client socket: " << socketToAccept << " being disconected and deleted..." << std::endl;
            // this->deleteConnection(socketToAccept); //TODO: Check if this is the right place to delete the connection
        }
    }
}

/**
 * @brief Main event loop. If a server socket receives a connection, it will call clientAccept. 
 * If a client socket is ready to read, it will call clientRead.
 * 
 * @return int 
 */
int Master::startEventLoop()
{
    struct epoll_event events[MAX_EVENTS];
    int nev;

    while (true)
    {
        printWaitConsole();
        
        nev = epoll_wait(_epoll_fd, events, MAX_EVENTS, 300); 
        
        if (nev == -1)
            ft_error("Error in epoll_wait");
        else if (nev > 0)
            manageConnections(events, nev);
    }
    return (0);
}

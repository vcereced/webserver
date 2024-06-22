/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgarizad <dgarizad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 20:12:40 by dgarizad          #+#    #+#             */
/*   Updated: 2024/06/22 13:52:14 by dgarizad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Connection.hpp"

Connection::Connection() : VHost(), _clientSocket(0), _clientAddrSize(sizeof(_clientAddr))
{
    _keepAlive = false;
    std::cout << BBLUE << "Connection constructor called" << RESET << std::endl;
}   

Connection::~Connection()
{
    std::cout << BRED << "Connection destructor called" << RESET << std::endl;
}

Connection::Connection(Connection const &src): VHost(src)
{
    *this = src;
}

Connection &Connection::operator=(Connection const &src)
{
    std::cout << BBLUE << "Connection operator= called" << RESET << std::endl;
    if (this != &src)
    {
        _clientSocket = src._clientSocket;
        _clientAddr = src._clientAddr;
        _clientAddrSize = src._clientAddrSize;
        _ev = src._ev;
        _buffer = src._buffer;
        // _requestedPathNoFile = src._requestedPathNoFile;
        // _finalPath = src._finalPath;
        // _requestedPath = src._requestedPath;
        // _fileName = src._fileName;
        // _queryString = src._queryString;
        // _root = src._root;
        _statusCode = src._statusCode;
        _keepAlive = src._keepAlive;
        _location = src._location;
        VHost::operator=(src);
    }
    return (*this);
}

Connection &Connection::operator=(VHost &src)
{
    if (this != &src)
    {
        this->setServer(src.getServerStruct());
    }
    return (*this);
}

RequestParser& Connection::getRequest(void)
{
    return _requestConnection;
}

bool Connection::getKeepAlive(void) const
{
    return _keepAlive;
}

int Connection::getClientSocket() const
{
    return _clientSocket;
}

std::string Connection::getBuffer() const
{
    return _buffer;
}

t_location  Connection::getLocation() const
{
    return _location;
}
std::string Connection::getPath() const
{
	return _path;
}

int Connection::getStatusCode() const
{
	return _statusCode;
}


std::string Connection::getFileName() const
{
    return _fileName;
}

// std::string Connection::getFinalPath() const
// {
//     return _finalPath;
// }

void Connection::setClientData(int clientSocket, sockaddr_in clientAddr, socklen_t clientAddrSize, struct epoll_event ev)
{
    _clientSocket = clientSocket;
    _clientAddr = clientAddr;
    _clientAddrSize = clientAddrSize;
    _ev = ev;
}

void Connection::setBuffer(std::string buffer)
{
    _buffer = buffer;
}

void Connection::setStatusCode(int statusCode)
{
    _statusCode = statusCode;
}





/**
 * @brief This function checks if the method sent by the client is 
 * allowed in the serverblock. 
 * @param request 
 * @return true 
 * @return false 
 */
bool Connection::methodCheck(std::string method)
{
    if (_location.allowedMethods[GET]    && method == "GET")    return true;
    if (_location.allowedMethods[POST]   && method == "POST") 	return true;
    if (_location.allowedMethods[PUT]    && method == "PUT") 	return true;
   	if (_location.allowedMethods[DELETE] && method == "DELETE") return true;

    return false;
}


//??????? CUIDADOOOO
std::string Connection::getMimeType(const std::string &path)//????????? default text y borrado octet CUIDADOOOOOO
{
    std::string mime;
    
	if (endsWith(path, ".html")) {
        mime = "text/html";
    } else if (endsWith(path, ".py")) {
        mime = "text/html";
    } else if (endsWith(path, "/")) {
        mime = "text/html";
    } else if (endsWith(path, ".css")) {
        mime = "text/css";
    } else if (endsWith(path, ".jpg") || endsWith(path, ".jpeg")) {
        mime = "image/jpeg";
    } else if (endsWith(path, ".png")) {
        mime = "image/png";
    } else if (endsWith(path, ".ico")) {
        mime = "image/x-icon";
    } else {
        // Default to application/octet-stream for unknown types
       mime = "application/octet-stream";
        //mime = "text/html";//???????
    }
    return mime;
}

void Connection::serveErrorPage(void)
{
    std::string         path = "./html/errorPages/";
    std::stringstream   buffer;
    std::string         response;
    std::string         response_header;
    std::string         response_body;
    
    if (this->getStatusCode() == 400)
        path += "400.html";
    else if (this->getStatusCode() == 403)
        path += "403.html";
	else if (this->getStatusCode() == 404)
        path += "404.html";
	else if (this->getStatusCode() == 405)
        path += "405.html";
    else if (this->getStatusCode() == 500)
        path += "500.html";

    std::ifstream       file(path);
    buffer << file.rdbuf();
    
    response_body = buffer.str();
    response_header = genHeaderHTTP(response_body, path);
    response = response_header + response_body;
    
    send(this->getClientSocket(), response.c_str(), strlen(response.c_str()), 0);
//    std::cout << GREEN << response << RESET << std::endl;
    showParamsConsoleHTTP(response, response.size(), this->getClientSocket(), this->getStatusCode(), true);
}

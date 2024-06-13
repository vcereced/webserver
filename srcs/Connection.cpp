/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgarizad <dgarizad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 20:12:40 by dgarizad          #+#    #+#             */
/*   Updated: 2024/06/08 15:13:28 by dgarizad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Connection.hpp"

Connection::Connection() : _clientSocket(0), _clientAddrSize(sizeof(_clientAddr))
{
}   

Connection::~Connection()
{
}

Connection::Connection(Connection const &src)
{
    *this = src;
}

Connection &Connection::operator=(Connection const &src)
{
    if (this != &src)
    {
        _clientSocket = src._clientSocket;
        _clientAddr = src._clientAddr;
        _clientAddrSize = src._clientAddrSize;
        _ev = src._ev;
        _buffer = src._buffer;
        // _vhost = src._vhost;
    }
    return (*this);
}

int Connection::getClientSocket() const
{
    return _clientSocket;
}

std::string Connection::getBuffer() const
{
    return _buffer;
}

std::string Connection::getPath() const
{
	return _path;
}

int Connection::getStatusCode() const
{
	return _statusCode;
}

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

void Connection::setVhost(VHost vhost)
{
    _vhost = vhost;
}


bool Connection::fileCheck(std::string file)
{
	if (_fileName.empty() == true)
	{
		std::cout << YELLOW "No file requested, serving index or autoindex" RESET << std::endl;
		_path += "/index.html"; //THIS IS HARDCODED! MANAGE THIS with the autoindex or index directive.
		return (true);
	}
	std::cout << BLUE "File requested: " RESET << file << std::endl;
	//print the working directory
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	std::cout << BLUE "Current working dir: " RESET << cwd << std::endl;
	std::ifstream f(file.c_str());
	if (f.good())
	{
		std::cout << GREEN "File found: 200!" RESET << std::endl;
		_statusCode = 200;
		return (true);
	}
	_statusCode = 404;
	std::cout << RED "File not found: 404!" RESET << std::endl;
	return (false);
}

/**
 * @brief this function checks if the uri is a defined location in the vhost configuration.
 * 
 * @param directory 
 * @return int 
 */
bool Connection::dirCheck(std::string directory)
{
    std::vector<t_location> locations = _vhost.getServerStruct().locations;
    for (std::vector<t_location>::iterator it = locations.begin(); it != locations.end(); it++)
    {
        if (it->uri == directory)
        {
            std::cout << MAGENTA "Location found:" RESET <<  it->uri << std::endl;
            _root = it->root;
			_statusCode = 200;
            return (true);
        }
    }
    // std::cout << RED "Location not found: 404!" RESET << directory << std::endl;
    _statusCode = 404;
	return (false);
}

/**
 * @brief This function fixes the path of the request.
 * It replaces the directory with the root of the location.
 * 
 * @param path 
 * @return int 
 */
int Connection::fixPath(std::string &path)
{
    size_t found = path.find(_directory);
    if (path == "/" && _fileName.empty() == false)
	{
		_root += "/";
	}
    if (found != std::string::npos)
    {
        path.replace(found, _directory.length(), _root);
    }
    else
    { //MAYBE THIS IS NOT NEEDED. IF THE DIRECTORY IS NOT FOUND, THE PATH WILL BE THE ROOT?
        std::cout << RED "Path not found: 404!" RESET << std::endl;
        _statusCode = 404;
		return (404);
    }
    return (0);
}

/**
 * @brief This is part of parsing the request. 
 * It checks if the uri is a directory or a file.
 * and sets the path of the file based on the root of the location.
 * @param request 
 * @return int 
 */
int Connection::uriCheck(RequestParser &request)
{
    std::string uri = request.get()["REQUEST_URI"];
    std::string::size_type pos;

    std::stringstream   iss(uri);
    std::getline(iss, _path, '?');
    std::getline(iss, _queryString, '?');
	
    std::cout << BLUE "_Path: " RESET << _path << std::endl;
    std::cout << BLUE "Query: " RESET << _queryString << std::endl;

    pos = _path.rfind('/');
    _directory = _path.substr(0, pos);
    _fileName = pos == std::string::npos ? _path : _path.substr(pos + 1);
    //if the _filename does not have an extension, it is a directory. recomplete the directory path
    if (_fileName.find('.') == std::string::npos)
    {
        _directory = _path;
        _fileName = "";
    }
	if (_directory == "")
		_directory = "/";
    std::cout << BLUE "Directory: " RESET << _directory << std::endl;
    //get just the first part of the _directory
    pos = _directory.find('/', 1);
    _finalPath = _directory;
    _directory = _directory.substr(0, pos);
    std::cout << BLUE "DirectoryParsed: " RESET << _directory << std::endl;
    std::cout << BLUE "_Filename: " RESET << _fileName << std::endl;
    if (dirCheck(_directory) == true)
        std::cout << GREEN "Directory found: 200!" RESET << std::endl;
    else
    {
        std::cout << RED "Directory not found: 404!" RESET << std::endl;
        return (404);
    }
    //print the root of the location
    std::cout << BLUE "Root: " RESET << _root << std::endl;
    if (fixPath(_finalPath) == 404 || fixPath(_path) == 404)
		return (404);
    std::cout << BLUE "Path fixed: " RESET << _finalPath << std::endl;
	std::cout << BLUE "full Path fixed: " RESET << _path << std::endl;
	if (fileCheck(_path) == false)
		return (404);
    //fix the path with the root of the location.
    return (0);
}
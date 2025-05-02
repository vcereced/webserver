/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Master.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 16:48:47 by dgarizad          #+#    #+#             */
/*   Updated: 2024/09/27 18:31:50 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Master.hpp"

Master::Master() 
{
}

Master::~Master() 
{
}

Master::Master(Master const &src)
{
	*this = src;
}

Master &Master::operator=(Master const &src)
{
	if (this != &src)
	{
		//copy the attributes here
	}

	return (*this);
}

void Master::setclientMaxBodySize(long n)
{
	this->_clientMaxBodySize = n;
}

long Master::getclientMaxBodySize(void)
{
	return this->_clientMaxBodySize;
}

std::map<int, std::string> &Master::getErrPages(void)
{
	return this->_errPageMap;
}


/**
 * @brief This function will return the VHost object for a given server name
 * it will iterate through the _vhosts vector and through the server_name vector of each VHost object
 * to find the correct one and return it. If the server name is not found, it will return the first VHost object
* as a default? or null? RESOLVE THIS!!!!. If servername is duplicated, it will return the first one found.
 *
 * @param serverName
 * @return VHost&
 */
VHost &Master::getVHost(std::string serverName, int port)
{
    std::vector<VHost>::iterator it = _vhosts.begin();
    for (; it != _vhosts.end(); it++)
    {
        std::vector<std::string> serverNames = (*it).getServerStruct().server_name;
        std::vector<std::string>::iterator it2 = serverNames.begin();
        for (; it2 != serverNames.end(); it2++)
        {
            if (*it2 == serverName && it->getServerStruct().listen == port)
                return (*it);
        }
    }
    throw std::runtime_error("getVHost: not found VHost!");
}

/**
 * @brief This function 
 * separates the hostport string into servername and port.
 * 
 * @param hostport 
 * @return VHost& 
 */
VHost &Master::assignVHost(std::string hostport)
{
    std::stringstream   iss(hostport);
    std::string         servername;
    std::string         tmp_port;
    int                 port;
    
	std::getline(iss, servername, ':');
    std::getline(iss, tmp_port, ':');
	// port = std::stoi(tmp_port); stoi is not supported in the c++98 standard
	std::stringstream(tmp_port) >> port;
    
	return this->getVHost(servername, port);
}

// AUXILIARY FUNCTIONS

//print server names, for debugging purposes. iterating through vhosts
// void Master::printServerNames()
// {
// 	std::cout << "Printing server names" << std::endl;
// 	for (int i = 0; i < _vhosts.size(); i++)
// 	{
// 		std::vector<std::string> serverNames = _vhosts[i].getServerStruct().server_name;
// 		std::vector<std::string>::iterator it2 = serverNames.begin();
// 		std::cout<< CYAN <<"HOST number: "<<RESET<< i << std::endl;
// 		for (; it2 != serverNames.end(); it2++)
// 		{
// 			std::cout<<YELLOW << "Server name: " <<RESET<< *it2 << std::endl;
// 		}
// 	}
// }

/**
 * @brief Here we will create the VHost objects for each server block in the configuration file
 * Also, we will populate the _vhostMap with the server name as the key and the VHost object as the value
 * Only one VHost object will be created for each server block, even if the server block has multiple server names.
 * @param config 
 * @return int 
 */
int Master::createVHosts(FileParse &config)
{
	std::vector<t_server> servers = config.getStruct().serverData;
	if (servers.size() == 0)
		throw std::runtime_error("No servers found in the configturation file");

	//iterate the vector and confirm that the server names are correctly stored
	std::vector<t_server>::iterator it = servers.begin();
	for(; it < servers.end(); ++it)
	{
		VHost vhost;
		vhost.setServer(*it);
		_vhosts.push_back(vhost);
		//populate the map with the server name as the key and the VHost object as the value
		std::vector<std::string>::iterator it2 = (*it).server_name.begin();
		for (; it2 != (*it).server_name.end(); it2++)
			_vhostMap[*it2] = vhost;
	}
	return (0);
}

void Master::setErrPages(std::map<int, std::string> errPagesMap)
{
	this->_errPageMap = errPagesMap;
}

void Master::manageError(int exceptionStatusCode, int socketToAccept)
{
	std::string errPagePath;
	
	errPagePath = this->getErrPages()[exceptionStatusCode];
	_clientsMap[socketToAccept].setStatusCode(exceptionStatusCode);
	_clientsMap[socketToAccept].serveErrorPage(errPagePath);
	this->deleteConnection(socketToAccept);
}


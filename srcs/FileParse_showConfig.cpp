/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileParse_showConfig.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 12:11:50 by vcereced          #+#    #+#             */
/*   Updated: 2024/10/02 10:04:34 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/FileParse.hpp"

void printPorts(FileParse *ptr)
{
    t_http &ref = ptr->getStruct();
    
    for (std::set<int>::iterator it = ref.ports.begin(); it != ref.ports.end(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

void printErrPages(FileParse *ptr)
{
    t_http &ref = ptr->getStruct();
    
    for (std::map<int, std::string>::iterator it = ref.errPageMap.begin(); it != ref.errPageMap.end(); ++it)
    {
        std::cout << "= " << it->first << " " << it->second << "\n\t\t\t\t";
    }
     std::cout << std::endl;
}
void printServerNames(std::vector<t_server>::iterator it)
{
    for (std::vector<std::string>::iterator itVec = it->server_name.begin(); itVec != it->server_name.end(); ++itVec)
    {
        std::cout << *itVec << " ";
    }
    std::cout << std::endl;
}

void printIndexNames(std::vector<t_location>::iterator itLoc)
{
    if (!itLoc->index.empty())
    {
        std::cout << "\t\t    /index\t= ";
        for (std::vector<std::string>::iterator itIndex = itLoc->index.begin(); itIndex != itLoc->index.end(); ++itIndex)
        {
            std::cout << *itIndex << " ";
        }
        std::cout << std::endl;
    }
}

void printAutoIndex(std::vector<t_location>::iterator itLoc)
{
    if (itLoc->autoIndex)
    {
        std::cout << "\t\t    /autoindex\t= " << itLoc->autoIndex << std::endl;
    }
}

void printRedirection(std::vector<t_location>::iterator itLoc)
{
    if (itLoc->redirection.statusCode != 0)
    {
        std::cout << "\t\t    /redirection= " << itLoc->redirection.statusCode << std::endl;
        std::cout << "\t\t\t\t= " << itLoc->redirection.urlRedirection << std::endl;
    }
}

void printCgi(std::vector<t_location>::iterator itLoc)
{
    if (!itLoc->cgiMap.empty())
    {
        std::cout << "\t\t    /cgi\t= ";
        for (std::map<std::string, std::string>::iterator itCgi = itLoc->cgiMap.begin(); itCgi != itLoc->cgiMap.end(); ++itCgi)
        {
            std::cout << itCgi->first << " : " << itCgi->second << "  " ;
        }
        std::cout << std::endl;
    }
}

void printLocations(std::vector<t_server>::iterator it)
{
    for (std::vector<t_location>::iterator itLoc = it->locations.begin(); itLoc != it->locations.end(); ++itLoc)
    {
        std::cout << "\t\t    /location\t= " << itLoc->location << std::endl;
        std::cout << "\t\t    /root\t= " << itLoc->root << std::endl;
        
        printIndexNames(itLoc);
        printAutoIndex(itLoc);
        
        std::cout << "\t\t    /methods\t= " << "GET:" << itLoc->allowedMethods[GET] << " POST:" << itLoc->allowedMethods[POST] << " PUT:" << itLoc->allowedMethods[PUT] << " DELETE:" << itLoc->allowedMethods[DELETE] << std::endl;
        
        printCgi(itLoc);
        printRedirection(itLoc);
        
        std::cout << std::endl << std::endl;
    }
}

void printServersData(FileParse *ptr)
{
    t_http &ref = ptr->getStruct();
    
    for (std::vector<t_server>::iterator it = ref.serverData.begin(); it != ref.serverData.end(); ++it)
    {
        std::cout << "           /server_name\t\t= " ;
        printServerNames(it);
        
        std::cout << "\t   /listen\t\t= " << it->listen << std::endl;
        
        std::cout << "\t   /locations/" << std::endl;
        printLocations(it);
        
    std::cout << "-------------------------------------------------------------------------" << std::endl;
    }
}

void FileParse::showConfig(void)
{
    std::cout << "/ports\t\t\t\t= " ;
    printPorts(this);
    
    std::cout << "/ClientMaxBodySize\t\t= " << this->configData.clientMaxBodySize << std::endl;
    
    std::cout << "/errorPages\t\t\t";
    printErrPages(this);
   
    std::cout << "/serverData/ " << std::endl;
    printServersData(this);

   
}
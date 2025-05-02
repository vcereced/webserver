/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection_requestCheck.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/19 10:19:40 by vcereced          #+#    #+#             */
/*   Updated: 2024/09/27 18:04:37 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Connection.hpp"

t_location Connection::getLocationVHost(std::string uriRequested)
{
    std::vector<t_location> locations;
    std::string             locationName;
    t_location              empty;
    
    locations = this->getServerStruct().locations;
    locationName = extractLocationUriStr(uriRequested);
    for (std::vector<t_location>::iterator it = locations.begin(); it != locations.end(); it++)
    {
        if (it->location == locationName)
        {
            this->_statusCode = OK;
            return *it;
        }
    }
    
    empty.location = "NULL";
    return empty;
}

std::string catEndSlashStr(std::string str)
{
    if (!str.empty() && str[str.length() - 1] != '/')
        str += "/";
    return str;
}

std::string locationSwapedRoot(Connection *ref, std::string uriRequested)
{
    std::string location;
    std::string root;

    location = extractLocationUriStr(uriRequested);
    location = catEndSlashStr(location);

    root = ref->getLocation().root;
    if (root.empty())
        return "";
    root = catEndSlashStr(root);
        
    uriRequested.replace(0, location.length(), root);
   
    return (uriRequested);
}

std::string cleanPathQuery(std::string path, std::string query)
{
    if (query.empty() == false)
        path.replace(path.find(query) - 1, query.length() + 1, "");// start -1 because Query dont have the '?', length + 1 to count the '?'
    
    else if (endsWith(path, "?"))
        path.erase(path.length() - 1);
        
    return path;
}

void Connection::requestParse(RequestParser &request)
{
    std::string pathSwapedWithQuery;
    std::string uriRequested;
    std::string method;
    
    uriRequested = request.get()["REQUEST_URI"];
    method = request.get()["REQUEST_METHOD"];

    if (method != "GET" && method != "POST" && method != "DELETE" )
        throw ServerException("requestParse: location requested method not allowed: " + uriRequested, METHOD_NOT_ALLOWED);
    this->_location = this->getLocationVHost(uriRequested);
    if (this->_location.location == "NULL")
        throw ServerException("requestParse: location requested wrong: " + uriRequested, NOT_FOUND);
        
    try {         
        this->_queryString  = extractQueryStr(uriRequested);
        
        this->_path         = cleanPathQuery(uriRequested, this->_queryString);
        
        this->_pathInfo     = extractPathInfo(this->_path);
        
        this->_path         = cleanPathInfo(this->_path, this->_pathInfo);
        
        this->_fileName     = extractFileNameStr(this->_path);
        
        this->_format       = extractExtension(this->_fileName);
        
        this->_path         = locationSwapedRoot(this,this->_path);
        
        showParamsConsole(uriRequested, this->_path, this->_fileName, this->_pathInfo, this->_queryString, this->_format);   
    } catch(const ServerException &e) {
        throw ServerException("requestParse: " + std::string(e.what()), INTERNAL_SERVER_ERROR);
    }
}


// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   Connection_genResponseCgi.cpp                      :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: vcereced <vcereced@student.42.fr>          +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2024/05/13 14:44:26 by dgarizad          #+#    #+#             */
// /*   Updated: 2024/06/20 09:49:07 by vcereced         ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */

 #include "../includes/Connection.hpp"

void Connection::setVarsEnviroment(RequestParser &request)
{
    const char *value;

    if (request.get()["REQUEST_METHOD"] == "GET")
    {
        value = this->_queryString.c_str();
        if (setenv("QUERY_STRING", value, 1) != 0)
            throw ServerException("setVarsEnviroment: QUERY_STRING error: " , INTERNAL_SERVER_ERROR);

        value = request.get()["REQUEST_METHOD"].c_str();
        if (setenv("REQUEST_METHOD", value, 1) != 0)
            throw ServerException("setVarsEnviroment: REQUEST_METHOD error: " , INTERNAL_SERVER_ERROR);

        value = this->_pathInfo.c_str();
        if (setenv("PATH_INFO", value, 1) != 0)
            throw ServerException("setVarsEnviroment: PATH_INFO error: " , INTERNAL_SERVER_ERROR);
    }
}

std::string Connection::genBodyCgi(std::string filePath, RequestParser &request)
{
    std::string cgi;// like /usr/bin/python3 or /usr/bin/bash
    std::string response_body;
    
    try{
        std::ifstream file;
        openFile(filePath, file);
        setVarsEnviroment(request);
        cgi = this->_location.cgiMap[this->_format]; //example:  .py : /usr/bin/python3
        response_body = readOutputCgi(cgi, filePath, this->_fileName);
        
    }catch(ServerException &e) {
        throw ServerException("genBodyCgi: " + std::string(e.what()), e.getCode());
    }

    return response_body;
}
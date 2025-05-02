/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   master_manageConnection.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcereced <vcereced@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 19:35:35 by dgarizad          #+#    #+#             */
/*   Updated: 2024/10/22 17:14:01 by vcereced         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Master.hpp"

void Connection::requestCheck(RequestParser &request)
{
    std::string method;
    std::string uriRequested;
       
    uriRequested = request.get()["REQUEST_URI"];
    
    method = request.get()["REQUEST_METHOD"];

    if (this->methodCheck(method) == false)
        throw ServerException("requestCheck: location requested method not allowed: " + uriRequested, METHOD_NOT_ALLOWED);
    if (this->_path.empty() && this->_location.redirection.statusCode == 0)
        throw ServerException("requestCheck: location requested have not root defined: " + uriRequested, INTERNAL_SERVER_ERROR);
    
    if (uriRequested.find("download") != std::string::npos)
    {
        //std::cout << BYELLOW << uriRequested + " se HA levantado el FLAG" << RESET << std::endl;
        this->_download = true;
    }
    else
    {
        //std::cout << RED << uriRequested + " NO se HA levantado el FLAG" << RESET << std::endl;
        this->_download = false;
    }
    
    if (method == "POST")
    {
        std::istringstream iss(request.get()["CONTENT_LENGTH"]);
        long contentLength;
        iss >> contentLength;
        if (contentLength > this->_clientMaxBodySize)
            throw ServerException("requestCheck: exceded max client body size " + uriRequested, PAYLOAD_TOO_LARGE);
    }
}

void Connection::Response(int clientSocket, const char * response, size_t size)
{
    send(clientSocket, response, size, 0);
}

/**
 * @brief get the buffer from the client object and process the request. 
 * 
 * @param clientSocket 
 * @return int 
 */
void Connection::processRequest(RequestParser &request)
{
    std::string response;

    std::cout << MAGENTA "PROCESSING REQUEST..." << RESET << std::endl;
    try{
        this->requestParse(request);
        this->requestCheck(request);
        response = this->genResponse(request);
    }catch(const ServerException &e) {
        throw ServerException("processRequest: " + std::string(e.what()), e.getCode());
    }
    //send(this->getClientSocket(), response.c_str(), response.size(), 0);
    this->Response(this->getClientSocket(), response.c_str(), response.size());
   
    showParamsConsoleHTTP(response, response.size(), this->getClientSocket(), this->getStatusCode(), false);
}


void Connection::readFromSocket(void)
{
    char    buffer[SOCKET_BUFFER_SIZE];
    int     bytesRead;
    int     clientSocket;

    clientSocket = this->getClientSocket();
    bytesRead = read(clientSocket, buffer, SOCKET_BUFFER_SIZE);
    if (bytesRead < 0)
    {
        std::ostringstream oss;
        oss << "readFromSocket: Error reading from client socket " << clientSocket;
       
        throw std::runtime_error(oss.str());
        
    }
    if (bytesRead == 0)
    {
        std::ostringstream oss;
        
        oss << "readFromSocket: 0 bytes read from client socket " << clientSocket;
        throw std::runtime_error(oss.str());    
    }
    else //store the buffer in the connection object which represent the client
    {
        _buffer2.resize(bytesRead);
        memcpy(_buffer2.data(), buffer, bytesRead); //for binary data
        buffer[bytesRead] = '\0';
        this->setBuffer(buffer);
    } 
}
//CHEKPOINT
void Master::manageConnection(Connection &connection)
{
    RequestParser&  request = connection.getRequest();
    VHost           VHostAssigned;
    std::string     buffer;

    try{
        connection.readFromSocket();
        buffer = connection.getBuffer();
        //Print buffer
       // std::cout << BYELLOW << "Buffer received from client: '" << RESET << std::endl;
        //std::cout << BCYAN << buffer <<"'"<< RESET << std::endl;
        //std::cout << BRED << "Buffer size: " << RESET << buffer.size() << std::endl;
        request.loadConfigFromRequest(buffer);

        request.showConfig();

        VHostAssigned = this->assignVHost(request.get()["HTTP_HOST"]);
        
        connection = VHostAssigned;         //post-herencia, clase BASE de Connection con la clase Base asignada

        connection.processRequest(request);
    }
    catch (const ServerException &e) {
        throw ServerException("manageConnection: " + std::string(e.what()), e.getCode());
    }
    catch (const std::exception &e) {
        throw std::runtime_error("manageConnection: " + std::string(e.what()));
        
    }
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   master_manageConnection.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgarizad <dgarizad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 19:35:35 by dgarizad          #+#    #+#             */
/*   Updated: 2024/06/22 13:54:26 by dgarizad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Master.hpp"

void Connection::requestCheck(RequestParser &request)
{
    std::string method;
    std::string uriRequested;
    
    uriRequested = request.get()["REQUEST_URI"];
    
    if (this->_path.empty())
	{
        throw ServerException("requestParse: location requested have not root defined: " + uriRequested, 500);
		//this->_statusCode = 404;
        //throw std::runtime_error("requestParse: location requested have not root defined: " + uriRequested);
	}
    
    method = request.get()["REQUEST_METHOD"];
    
    if (this->methodCheck(method) == false)
	{
        throw ServerException("requestParse: location requested method not allowed: " + uriRequested, 405);
		//this->_statusCode = 405;
        //throw std::runtime_error("requestParse: location requested method not allowed: " + method);
        
	}
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
    send(this->getClientSocket(), response.c_str(), response.size(), 0);
    //close(this->getClientSocket());// IMPORTANT TO DEFINE!! ???????????????????????????????????????????????????? CREO QUE NO: ES AL SALIR DE MANAGE CONNECTION PUES FINALIZO AUNQUE ES LO MISMO HAY QUE AGRUPAR las acciones de LIMPIAR LOS VECTOR FDS

    showParamsConsoleHTTP(response, response.size(), this->getClientSocket(), this->getStatusCode(), false);
}


void Connection::readFromSocket(void)
{
    char    buffer[BUFFER];//????????????????????????????????????????????limitante para el upload???????????????
    int     bytesRead;
    int     clientSocket;

    clientSocket = this->getClientSocket();
    bytesRead = read(clientSocket, buffer, BUFFER);
    
    if (bytesRead < 0)
        throw std::runtime_error("readFromSocket: Error reading from client socket " + std::to_string(clientSocket));  //REMOVE CLIENT SOCKET FROM EPOLL SET AND CLOSE SOCKET!

    if (bytesRead == 0)
        throw std::runtime_error("readFromSocket: 0 bytes read from client socket " + std::to_string(clientSocket));
    else //store the buffer in the connection object which represent the client
    {
        buffer[bytesRead] = '\0';
        this->setBuffer(buffer);
    } 
}
//CHEKPOINT
void Master::manageConnection(Connection &connection)
{
    RequestParser&   request = connection.getRequest();
    VHost           VHostAssigned;
    std::string     buffer;

    try{
        connection.readFromSocket();
        buffer = connection.getBuffer();
        //Print buffer
        std::cout << BYELLOW << "Buffer received from client: " << RESET << std::endl;
        std::cout << BCYAN << buffer << RESET << std::endl;
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

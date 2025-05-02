/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection_genResponsePOST.cpp                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 10:21:56 by marvin            #+#    #+#             */
/*   Updated: 2024/10/02 10:39:35 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Connection.hpp"


std::string Connection::genResponsePOST(RequestParser &request)
{
    std::string response;
    
    this->_keepAlive = true;

        if (request.getBodyWatchDog() == 1)
        {
            this->processPost();

            response = "HTTP/1.1 200 OK\r\n";
            response += "Content-Type: text/plain\r\n";
            response += "Content-Length: 22\r\n"; // Length of "POST request processed"
            response += "\r\n";
            response += "POST request processed";
            _keepAlive = false;
            return response;
        }
        
        std::string continueRequest = "HTTP/1.1 100 Continue\r\n\r\n";
        return continueRequest;
}
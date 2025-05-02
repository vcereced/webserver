/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection_delete.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgarizad <dgarizad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 23:38:25 by vcereced          #+#    #+#             */
/*   Updated: 2024/07/07 13:29:17 by dgarizad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Connection.hpp"
#include <iostream>
#include <filesystem>
#include <cstdio>



std::string responseDELETE(Connection *ref)
{
    std::string responseHTTP_header;
    std::string responseHTTP_body;
    
    if (std::remove(ref->getPath().c_str()) == 0)
    {
        ref->setStatusCode(NO_CONTENT);
        
        responseHTTP_body = ref->getFileName() + "deleted succesfully, try rm -rf /* ja";
        responseHTTP_header = ref->genHeaderHTTP(responseHTTP_body, ref->getPath());
        
        return responseHTTP_header + responseHTTP_body;
    } 
    else
        throw ServerException("genResponseDELETE: internal error: " + ref->getPath(), INTERNAL_SERVER_ERROR);
}


std::string Connection::genResponseDELETE()
{
    std::string responseHTTP;

    try{
        
        if (this->_fileName.empty() == true)
            throw ServerException("no filename to delete ", BAD_REQUEST);
        
        std::ifstream file;
        openFile(this->getPath(), file);
        
        if (this->_fileName.empty() == false)
            responseHTTP = responseDELETE(this);
            
    }catch (const ServerException &e) {
        throw ServerException("genResponseDELETE: " + std::string(e.what()), e.getCode());
    }
    return responseHTTP;
}
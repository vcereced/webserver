/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   master2.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcereced <vcereced@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 19:35:35 by dgarizad          #+#    #+#             */
/*   Updated: 2024/06/15 20:05:38 by vcereced         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Master.hpp"


int isAutoIndex(Connection &connection)
{
	if (connection.getAutoIndex() == true && connection.getFileName().empty())
		return 1;
	return 0;
}

/**
 * @brief get the buffer from the client object and process the request. 
 * 
 * @param clientSocket 
 * @return int 
 */
int Master::processRequest(Connection &connection, RequestParser &request)
{
	
    std::cout << MAGENTA "Processing request" << RESET << std::endl;
	
	if (connection.requestCheck(request) == -1)
		return -1;
	
	if (isAutoIndex(connection))
	{
		std::string response = connection.genAutoIndex(connection.getFinalPath()) ;
		send(connection.getClientSocket(), response.c_str(), strlen(response.c_str()), 0);
	}
	else
	{
		connection.servePage(connection.getPath());
	}
	

	
	// 	genAutoIndex(connection.getFinalPath()) ;
	
	// 	send (verveePage)
	// AUTOINDEX
	// servirpagina
	// cgi
    //if (connection.uriCheck(request) != 200)
	// {
	// 	std::cout << RED "Error in uriCheck" RESET << std::endl;
	// 	connection.servePage("./html/errorPages/404.html");
	// 	return (-1);
	// }
	// if (connection.methodCheck(request) == false)
	// {
	// 	std::cout << RED "Error in Method check" RESET << std::endl;
	// 	connection.servePage(".html/errorPages/405.html");
	// 	return (-1);
	// }
	
	//connection.servePage(connection.getPath());
	
    // close(clientSocket); //THIS IS IMPORTANT TO DEFINE!!
    std::cout << GREEN "Response sent" RESET << std::endl;

    return (0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 21:41:19 by dgarizad          #+#    #+#             */
/*   Updated: 2024/09/27 19:02:06 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"
#include "../includes/Master.hpp"
#include "../includes/FileParse.hpp"
#include "../includes/VHost.hpp"


int main(int argc, char **argv)
{
	FileParse 	fileParser;
	Master 		master;
	std::string path;
	
	if (argc > 2)
		return (ft_error("Bad arguments, use: ./webserv [config_file]"));	
	else if (argc == 2 )
		path = argv[1];
	else
		path = "./config/default.com";

	std::cout << GREEN << "Parsing configuration file: " << path << RESET<< std::endl;
	
	try{		
		fileParser.loadConfigFromFile(path);
	} catch (std::exception &e){
		return ft_error("Exception: main: "+ std::string(e.what()));
	}

	fileParser.showConfig();
	master.createVHosts(fileParser);
	master.setSockets(fileParser.getStruct().ports);
	master.setErrPages(fileParser.getStruct().errPageMap);
	master.setclientMaxBodySize(fileParser.getStruct().clientMaxBodySize);
	master.setEvents();
	master.startEventLoop();
	return (0);
}
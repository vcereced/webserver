/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VHost.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcereced <vcereced@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 15:17:12 by dgarizad          #+#    #+#             */
/*   Updated: 2024/06/19 09:36:11 by vcereced         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VHOST_HPP
# define VHOST_HPP
# include "webserv.hpp"
# include "FileParse.hpp"

class FileParse;

class VHost
{
	public:
		VHost();
		VHost(FileParse &config);
		~VHost();
		VHost(VHost const &src);
		VHost &operator=(VHost const &src);

		//getters
		t_server &getServerStruct();
		std::string getServerName();
		
		//setters
		void setServer(t_server &server);
		//AUXILIARY FUNCTIONS
		void printServerNames();

		//VHOST WILL HAVE THE METHODS 

	protected:
		//THIS IS THE SERVER BLOCK STRUCTURE
		t_server _server;
		
	//having a static variable for storing all the VHosts sockets in a set
	static std::set<int> _ListenSockets;
	
};
#endif
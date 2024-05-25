/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Master.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgarizad <dgarizad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 16:11:34 by dgarizad          #+#    #+#             */
/*   Updated: 2024/05/19 18:32:19 by dgarizad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MASTER_HPP
# define MASTER_HPP
# define MAX_CLIENTS 100
#include "webserv.hpp"

class VHost;

class Master
{
	public:
		Master();
		~Master();
		Master(Master const &src);
		Master &operator=(Master const &src);

		int createVHosts(FileParse &config);
		
		//NETWORK METHODS
		int setSockets(std::set<int> &ports);
		int setEvents();
		int startEventLoop();

		//FINDS AND RETURNS THE VHOST OBJECT FOR A GIVEN SERVER NAME
		VHost &getVHost(std::string serverName);
		

		//AUXILIARY FUNCTIONS
		void printServerNames();
		
	private:

		std::vector<int> _ListenSockets;
		std::vector<VHost> _vhosts;
		std::vector<t_server> _servers;
		std::map<std::string, VHost> _vhostMap;
		int _kq;
		int _epoll_fd;
};


#endif
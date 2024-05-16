/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VHost.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgarizad <dgarizad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 15:11:51 by dgarizad          #+#    #+#             */
/*   Updated: 2024/05/16 18:11:19 by dgarizad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/VHost.hpp"

VHOST::VHOST()
{
}

VHOST::VHOST(ConfParse &config)
{
	//parse the configuration and store it in the VHost object
}

VHOST::~VHOST()
{
}

VHOST::VHOST(VHOST const &src)
{
	*this = src;
}

VHOST &VHOST::operator=(VHOST const &src)
{
	if (this != &src)
	{
		//copy the attributes here
	}
	return (*this);
}


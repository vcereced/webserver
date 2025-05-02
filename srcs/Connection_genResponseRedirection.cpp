/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection_genResponseRedirection.cpp              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 10:53:53 by marvin            #+#    #+#             */
/*   Updated: 2024/10/02 15:20:05 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Connection.hpp"

std::string Connection::genResponseRedirection(void)
{
    std::string responseHTTP;

    if (this->_location.redirection.statusCode == MOVED_PERMANENTLY)
    {
        responseHTTP = "HTTP/1.1 301 Moved Permanently\r\n";
        responseHTTP += "Location: ";
        responseHTTP += this->_location.redirection.urlRedirection;
        responseHTTP += "\r\n\r\n";
    }

    return responseHTTP;
}

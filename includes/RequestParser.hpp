/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 17:28:48 by vcereced          #+#    #+#             */
/*   Updated: 2024/09/27 19:02:01 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP
# include "webserv.hpp"
# include <string>
# include <iostream>
# include <sstream>
# include <exception>
# include <map>

# define RED "\033[31m"
# define GREEN "\033[32m"
# define YELLOW "\033[33m"
# define BLUE "\033[34m"
# define MAGENTA "\033[35m"
# define RESET "\033[0m"
//bright colors
# define BRED "\033[91m"
# define BGREEN "\033[92m"
# define BYELLOW "\033[93m"
# define BBLUE "\033[94m"
# define BMAGENTA "\033[95m"
# define BCYAN "\033[96m"


class RequestParser {
public:
	//Constructors
	RequestParser(void) {
		_headerWatchDog = 0;
		_bodyWatchDog = 0;
		_multiPart = false;
	}
	RequestParser(RequestParser const &src);
	RequestParser &operator=(RequestParser const &src);

	//destructor
	~RequestParser() {};

	void loadConfigFromRequest(const std::string );
	void showConfig(void);

	//setters
	void set(std::string, std::string);
	
	//getters
	std::map<std::string, std::string> &get(void);
	std::string							getRequestHeader(void) const;
	std::string							getRequestBody(void) const;
	int 								getHeaderWatchDog(void) const;
	int 								getBodyWatchDog(void) const;
	std::string							getPostFileName(void) const;
	std::string							getBoundary(void) const;
	bool 								getMultiPart(void) const;
	
	//Methods
	void lineParser(RequestParser *ref, std::string &requestLine);
	void contenTypeParse(RequestParser *ref, std::istringstream &iss);
	void contentDispositionParse(RequestParser *ref, std::istringstream &iss);

private:
    std::map<std::string, std::string> _requestData;
	std::string						   _requestHeader;
	std::string						   _requestBody;
	int								   _headerWatchDog;
	int 							   _bodyWatchDog;
	//POST
	std::string						   _boundary;
	std::string						   _postFileName;
	bool							   _multiPart;
};

#endif  // RequestParse_HPP 
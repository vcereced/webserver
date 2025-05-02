/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 21:31:14 by dgarizad          #+#    #+#             */
/*   Updated: 2024/10/02 15:19:55 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//HERE WE WILL INCLUDE ALL THE HEADER FILES NEEDED FOR THE WEB SERVER
#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <string>
# include <vector>
# include <map>
# include <fstream>
# include <sstream>
# include <sys/socket.h>
# include <sys/stat.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
// #include <sys/event.h> // Include this for kqueue
# include <sys/time.h> 
# include <set>
# include <cstring>
# include <algorithm>
# include <sys/epoll.h>
# include <fcntl.h>
# include <iostream>
# include <cstdio>
# include <iomanip>
# include <sys/wait.h>
# include <signal.h>
# include <filesystem>
 
 
# define RED 						"\033[31m"
# define GREEN 						"\033[32m"
# define YELLOW 					"\033[33m"
# define BLUE 						"\033[34m"
# define MAGENTA 					"\033[35m"
# define CYAN 						"\033[36m"
# define RESET 						"\033[0m"
# define PINK 						"\033[95m"
 

//DEFAULT CONFIGURATION!!!!
# define MAX_EVENTS 				64 
# define SOCKET_BUFFER_SIZE  		420000 //default limit body size if not configured in .conf ??????
# define CGI_BUFFER_SIZE			10000
			
			
# define ERROR_PAGES 				"./html/errorPages/"
# define OK 						200
# define NO_CONTENT 				204
# define MOVED_PERMANENTLY			301
# define BAD_REQUEST 				400
# define FORBIDDEN 					403
# define NOT_FOUND 					404
# define METHOD_NOT_ALLOWED 		405
# define PAYLOAD_TOO_LARGE			413
# define INTERNAL_SERVER_ERROR 		500
 
 
# define BAD_REQUEST_FILE 			"400.html"
# define FORBIDDEN_FILE 			"403.html"
# define NOT_FOUND_FILE 			"404.html"
# define METHOD_NOT_ALLOWED_FILE 	"405.html"
# define PAYLOAD_TOO_LARGE_FILE 	"413.html"
# define INTERNAL_SERVER_ERROR_FILE "500.html"
 

# define ALLOW_AUTOINDEX 			false
# define ALLOW_GET 					true
# define ALLOW_POS 					true
# define ALLOW_PUT 					true
# define ALLOW_DELETE 				false

enum server {
	LISTEN,
	SERVER_NAME,
	LOCATION,
	TOTAL_SERVER
};

enum methods {
	GET,
	POST,
	PUT,
	DELETE,
	TOTAL_METHODS
};

typedef struct s_redirection {
	int									statusCode;
	std::string							urlRedirection;
}t_redirection;

typedef struct s_location {
	std::string							location;
	std::string							root;
	std::vector<std::string>			index;
	std::map<std::string, std::string>	cgiMap;
	bool								autoIndex;
	bool								allowedMethods[TOTAL_METHODS];
	t_redirection						redirection;
}t_location;

typedef struct s_server {
    std::vector<std::string>			server_name;
    int 								listen;
    int 								port;
    std::vector<t_location>				locations;

} t_server;

typedef struct s_fileParse {
	std::set<int>						ports;
	std::map<int, std::string>			errPageMap;
	long								clientMaxBodySize;
	std::vector<t_server>				serverData;
}t_http;

class ServerException: public std::exception {
public:
    ServerException(const std::string& msg, int c) : message(msg), code(c) {}
	virtual ~ServerException() throw() {} 
	//ServerException(const std::string& msg) : message(msg), code(0) {}
    // Sobrescribir el método what() para proporcionar un mensaje de error
    const char* what()  const throw() {
        return message.c_str();
    }

    // Método para obtener el código de error
    int getCode() const {
        return code;
    }

private:
    std::string message;
    int code;
};

//UTILS
int 			ft_error(std::string msg);
std::string 	extractLocationUriStr(std::string uri);
std::string 	extractFileNameStr(std::string uri);
std::string 	extractQueryStr(std::string uri);
std::string 	extractPathInfo(std::string uri);
std::string 	extractExtension(std::string fileName);
std::string 	cleanPathInfo(std::string path, std::string query);
bool        	endsWith(const std::string &str, const std::string &ending);
std::string 	readOutputCgi(std::string cgi, std::string filePath, std::string file);
void 			showParamsConsole(std::string &, std::string &, std::string &, std::string &, std::string &, std::string &);
void 			showParamsConsoleHTTP(std::string , size_t , int, int, bool);
void 			printWaitConsole(void);
char** 			convertToCharArray(const std::vector<std::string>& strList); 
void 			openFile(std::string filePath, std::ifstream& file);
int				ftFreePointerArr(char **arr);
bool 			isNumber(std::stringstream &ss);

#endif
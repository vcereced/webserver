/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileParse.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 12:12:05 by vcereced          #+#    #+#             */
/*   Updated: 2024/10/02 15:57:36 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "../includes/FileParse.hpp"


int ft_err(std::string str)
{
    std::cerr << RED << str << RESET << std::endl;
    return -1;
}

void ft_braces(int &braces, std::string &token)
{
    if (token == "{")
        braces++;
    else if (token == "}")
        braces--;
}

void setDefaultValuesLocation(t_location &ref)
{
    ref.autoIndex                   = ALLOW_AUTOINDEX;
    ref.allowedMethods[GET]         = ALLOW_GET;
    ref.allowedMethods[POST]        = ALLOW_POS;
    ref.allowedMethods[PUT]         = ALLOW_PUT;
    ref.allowedMethods[DELETE]      = ALLOW_DELETE;
    ref.redirection.statusCode      = 0;
    ref.redirection.urlRedirection  = "";
}

void setDefaultParams(t_http &ref)
{
    std::string path =  ERROR_PAGES;

    ref.errPageMap[BAD_REQUEST] = path + BAD_REQUEST_FILE;
    ref.errPageMap[FORBIDDEN] = path + FORBIDDEN_FILE;
    ref.errPageMap[NOT_FOUND] = path + NOT_FOUND_FILE;
    ref.errPageMap[METHOD_NOT_ALLOWED] = path + METHOD_NOT_ALLOWED_FILE;
    ref.errPageMap[PAYLOAD_TOO_LARGE] = path + PAYLOAD_TOO_LARGE_FILE;
    ref.errPageMap[INTERNAL_SERVER_ERROR] = path + INTERNAL_SERVER_ERROR_FILE;

    ref.clientMaxBodySize = SOCKET_BUFFER_SIZE;
}

void setNotAllowedMethod(t_location &ref, std::istringstream &iss)
{
	std::string token;

	while (iss >> token)
	{
		token.erase(std::remove(token.begin(), token.end(), ';'), token.end());

		if 		(token == "GET") 	ref.allowedMethods[GET] 	= false;
    	else if (token == "POST") 	ref.allowedMethods[POST] 	= false;
    	else if (token == "PUT") 	ref.allowedMethods[PUT] 	= false;
   		else if (token == "DELETE") ref.allowedMethods[DELETE] 	= false;
		else throw std::invalid_argument("not valid method " + token);
	}
}

void setAllowedMethod(t_location &ref, std::istringstream &iss)
{
    std::string token;

	while (iss >> token)
	{
		token.erase(std::remove(token.begin(), token.end(), ';'), token.end());

		if 		(token == "GET") 	ref.allowedMethods[GET] 	= true;
    	else if (token == "POST") 	ref.allowedMethods[POST] 	= true;
    	else if (token == "PUT") 	ref.allowedMethods[PUT] 	= true;
   		else if (token == "DELETE") ref.allowedMethods[DELETE] 	= true;
		else throw std::invalid_argument("not valid method " + token);
	}
}
void setCgiExtensions(t_location &ref, int &nServer, int &nLocation, std::istringstream &iss)
{
    std::string key;
    std::string value;

    iss >> key;
    key.erase(std::remove(key.begin(), key.end(), ';'), key.end());

    iss >> value;
    value.erase(std::remove(value.begin(), value.end(), ';'), value.end());
	
    ref.cgiMap[key] = value;
    nServer *= 1;
    nLocation *= 1;
}

void setRedirectionStatusCode(t_location &ref, std::string token)
{
    int                 statusCode;
    std::stringstream   ss(token);
    
    if (!(ss >> statusCode) || !isNumber(ss))
        throw std::invalid_argument("not valid status code of redirection " + token);
    else
        ref.redirection.statusCode = statusCode; 
}

void setRedirectionUrl(t_location &ref, std::string token)
{
    ref.redirection.urlRedirection = token;
}
void setRedirection(t_location &ref, std::istringstream &iss)
{
    std::string token;
    
	iss >> token;
    token.erase(std::remove(token.begin(), token.end(), ';'), token.end());
    
    setRedirectionStatusCode(ref, token);

    iss >> token;
    token.erase(std::remove(token.begin(), token.end(), ';'), token.end());
    
    setRedirectionUrl(ref, token);
    
    if ((iss >> token))
        throw std::invalid_argument("no valid redirection configuration");
}

void varLocation(FileParse *ref, std::istringstream &iss, int &nServer, int &nLocation, std::string &token)
{
    if (token == "root" )
    {
        iss >> token; 
        token.erase(std::remove(token.begin(), token.end(), ';'), token.end());
        ref->getStruct().serverData[nServer].locations[nLocation].root = token;
    }
    else if (token == "index")
    {
        while (iss >> token)
        {
            token.erase(std::remove(token.begin(), token.end(), ';'), token.end());
            ref->getStruct().serverData[nServer].locations[nLocation].index.push_back(token);
        }
    }
    else if (token == "autoindex")
    {
        iss >> token;
        token.erase(std::remove(token.begin(), token.end(), ';'), token.end());
        if (token == "on" )
            ref->getStruct().serverData[nServer].locations[nLocation].autoIndex = true;
    }
	else if (token == "not_allow")
		setNotAllowedMethod(ref->getStruct().serverData[nServer].locations[nLocation], iss);
    else if (token == "allow")
        setAllowedMethod(ref->getStruct().serverData[nServer].locations[nLocation], iss);
    else if (token == "cgi")
        setCgiExtensions(ref->getStruct().serverData[nServer].locations[nLocation], nServer, nLocation, iss);
    else if (token == "return")
        setRedirection(ref->getStruct().serverData[nServer].locations[nLocation], iss);
}

void insideLocation(FileParse *ref,std::ifstream &file, std::istringstream &iss, int &nServer, int &nLocation, std::string &token)
{
    int 	        locationBraces = 0;
    std::string     line;
    t_location      empty;

    if (token == "location")
    {
        //push a empty t_location into locations vector
        nLocation++;
        ref->getStruct().serverData[nServer].locations.push_back(empty);
        //register the location
        iss >> token;
        ref->getStruct().serverData[nServer].locations[nLocation].location = token;
        setDefaultValuesLocation(ref->getStruct().serverData[nServer].locations[nLocation]);
        do{
            while(iss >> token)
		    {
			    ft_braces(locationBraces, token);
                varLocation(ref, iss, nServer, nLocation, token);
		    }
            if (locationBraces > 0 && std::getline(file, line))
            {
                iss.clear();    
                iss.str(line);
            }
        }while (locationBraces > 0);
    }
}

void varServer(FileParse *ref, std::ifstream &file, std::istringstream &iss, int &nServer, int &nLocation, std::string &token, bool *vars)
{
    int port;

    if (token == "listen" && (vars[LISTEN] = true))
    {
        iss >> token;
        token.erase(std::remove(token.begin(), token.end(), ';'), token.end());
        // port = std::stoi(token); //if error it throw exception
        std::stringstream ss(token);
        if (!(ss >> port) || !isNumber(ss))
            throw std::runtime_error("port is not a number");
        ref->getStruct().ports.insert(port);
        ref->getStruct().serverData[nServer].listen = port;
    }
    else if (token == "server_name" && (vars[SERVER_NAME] = true))
    {
       while (iss >> token)
        {
            token.erase(std::remove(token.begin(), token.end(), ';'), token.end());
            ref->getStruct().serverData[nServer].server_name.push_back(token);
        }
    }
    else if (token == "location" && (vars[LOCATION] = true))
        insideLocation(ref, file, iss, nServer, nLocation, token);
}

void insideServer(FileParse *ref, std::ifstream &file, std::istringstream &iss, std::string &token)
{
    static int      nServer = -1;
    int             nLocation = -1;
    int 	        serverBraces = 0;
    bool            vars[TOTAL_SERVER] = {false, false, false};//to markdown the vars to find
    std::string     line;
    t_server        empty;

    if (token == "server")
    {
        //push a empty t_server in server vector
        nServer++;
        ref->getStruct().serverData.push_back(empty);
       do{
            while(iss >> token)
		    {
			    ft_braces(serverBraces, token);
                varServer(ref, file, iss, nServer, nLocation, token, vars);
		    }
            if (serverBraces > 0 && std::getline(file, line))
            {
                iss.clear();    
                iss.str(line);
            }
        }while (serverBraces > 0);
        //Checks if vars are encounter inside of server block
        if (vars[LISTEN] == false || vars[SERVER_NAME] == false || vars[LOCATION] == false)
            throw std::runtime_error("Server block wrong");
    }
}

void setClientMaxBodySize(FileParse *ref, std::istringstream &iss)
{
    std::string token;
    long        n;

    iss >> token;
    token.erase(std::remove(token.begin(), token.end(), ';'), token.end());
    // n = std::stoi(token);
    std::stringstream ss(token);
    if (!(ss >> n))
        throw std::runtime_error("client_max_body_size is not a number");
    ref->getStruct().clientMaxBodySize = n;
}

void setErrPages(FileParse *ref, std::istringstream &iss)
{
    std::string token;
    int         errN;
    
    iss >> token;
    // errN = std::stoi(token);
    std::stringstream ss(token);
    if (!(ss >> errN))
        throw std::runtime_error("error_page is not a number");
    iss >> token;
    token.erase(std::remove(token.begin(), token.end(), ';'), token.end());
    ref->getStruct().errPageMap[errN] = token;
}

void varHttp(FileParse *ref, std::ifstream &file, std::istringstream &iss, std::string &token, bool &server)
{
    if (token == "server" && (server = true))
        insideServer(ref, file, iss, token);
    if (token == "error_page")
        setErrPages(ref, iss);
    if (token == "client_max_body_size")
        setClientMaxBodySize(ref, iss);
}


/**
 * @brief iter if Http block till close the block. if http encounter start to process all 
 * line and its token / braces. if server is encounter it jump into it, if not encounter
 * throw a exception.
 * @return 1 if any http block is encounter. -1 if not.
 */
bool insideHttp(FileParse *ref, std::ifstream &file, std::istringstream &iss, std::string &token)
{
	int 	        httpBraces = 0;
	int 		    http = false;
    bool            server = false;
    std::string     line;

	if (token == "http" && (http = true))
    {
        setDefaultParams(ref->getStruct());
        do{
            while(iss >> token)
		    {
			    ft_braces(httpBraces, token);
                varHttp(ref, file, iss, token, server);
            }   
            if (httpBraces > 0 && std::getline(file, line))
            {
                iss.clear();    
                iss.str(line);
            }
        }while (httpBraces > 0);
        
        if (!server)
            throw std::runtime_error("Http block wrong");
    }

	return http;
}


/**
 * @brief iter the tokens of the line, then call to function according to server block or not.
 */
int lineParser(FileParse *ref, std::ifstream &file, std::string &line)
{
    std::istringstream  iss(line);
    std::string         token;
   
    //Process the first toke of the line
    iss >> token;

    return (insideHttp(ref, file, iss, token));//jump into if "http"
}

/**
 * @brief try to open the file, then iter the characteres of all lines counting the '{' '}'
 * @return int 0 if {} are close proferly, -1 if doesnt match
 */
int checksBraces(std::string filename)
{
    std::ifstream       file(filename.c_str());
    std::string         line;
    int                 braces = 0;

    if (!file.is_open())
        throw std::runtime_error("checksBraces: " + filename);
    while (std::getline(file, line))
    {
        for (size_t i = 0; i < line.size(); ++i)
        {
            if (line[i] == '{')
                braces++;
            else if (line[i] == '}')
                braces--;
        }
    }
    file.close();
    if (braces != 0)
		throw std::runtime_error("checksBraces: braces not match");
    return 0;
}

/**
 * @brief Getter the reference of the structure atribute of FileParse 
 * @return reference to t_http of instance 
 */
t_http & FileParse::getStruct()
{
    return this->configData;
}

/**
 * @brief Method to load on ifstream object the full config file. Iter line per line parsing the file
 * @return int -1 cannot open file or any exception. 0 ok.
 */
void FileParse::loadConfigFromFile(const std::string filename)
{
    std::ifstream       file(filename.c_str());
    std::string         line;
    bool                http = false;
    
	try {
        
		checksBraces(filename);
        if (!file.is_open())
            throw std::runtime_error("loadConfigFromFile cannot open " + filename);
        while (std::getline(file, line)) 
        {  
            if (lineParser(this, file, line) == true)//Parse line per line
                http = true; 
        }

        file.close();
        
        if (http == false)
            throw std::runtime_error("not found http block " + filename);

	}catch(const std::exception& e) {
        throw std::runtime_error("loadConfigFromFile: " +  std::string(e.what()));
	} 
}


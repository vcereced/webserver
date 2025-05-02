/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 18:04:58 by vcereced          #+#    #+#             */
/*   Updated: 2024/09/27 19:03:10 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/RequestParser.hpp"


void LineParseConcat(RequestParser *ref, std::istringstream &iss, std::string &token)
{
    std::string     tokenconcated;
    std::string     token2;

    while (iss >> token2)
        tokenconcated += token2;
    ref->set(token, tokenconcated);
}

/**
 * @brief just parsing the line of Request starting by "User-Agent:" 
 */
void userAgentLineParse(RequestParser *ref, std::istringstream &iss)
{
    std::string     token;
    std::string     tokenconcated;

    while (iss >> token)
        tokenconcated += token;
    ref->set("HTTP_USER_AGENT", tokenconcated);
}

/**
 * @brief just parsing the line of Request starting by a POST/PUT/DELETE/GET" 
 */
void receivedLineParse(RequestParser *ref, std::istringstream &iss, std::string &token)
{
    ref->set("REQUEST_METHOD", token);      //first token was extracted in previous funcion
    iss >> token;                           //extract next token from line in istringstream
    ref->set("REQUEST_URI", token); //HERE WE FIXED THE NAME FROM QUERY_STRING TO REQUEST_URI
    iss >> token;
    ref->set("SERVER_PROTOCOL", token);
}

std::string extractString1(const std::string& content, std::string str) {
    std::istringstream tokenStream(content);
    std::string token;
    
    while (std::getline(tokenStream, token, ';')) {
        std::string::size_type pos = token.find(str);
        if (pos != std::string::npos) {
            // Extract the substring after the found position
            std::string extracted = token.substr(pos + str.length());
            // Check if the last character is a quote and remove it if so
            if (!extracted.empty() && extracted[extracted.length() - 1] == '"') {
                extracted.erase(extracted.length() - 1);
            }
            return extracted;
        }
    }
    return ""; // Return empty string if not found
}

void RequestParser::contenTypeParse(RequestParser *ref, std::istringstream &iss)
{
    std::string     token;
    std::string     tokenconcated;

    while (iss >> token)
        tokenconcated += token;
    ref->set("CONTENT_TYPE", tokenconcated);

    if (tokenconcated.find("multipart/form-data") != std::string::npos)
        _multiPart = true;
    if (tokenconcated.find("boundary=") != std::string::npos)
    {
        _boundary = extractString1(tokenconcated, "boundary=");
    }
   
}


void RequestParser::contentDispositionParse(RequestParser *ref, std::istringstream &iss)
{
    std::string     token;
    std::string     tokenconcated;

    while (iss >> token)
        tokenconcated += token;
    ref->set("CONTENT_DISPOSITION", tokenconcated);

    if (tokenconcated.find("filename=") != std::string::npos)
    {
        _postFileName = extractString1(tokenconcated, "filename=\"");
    }
}

bool isTheEnd(std::string &line)
{
    if (line == "\r\n")
        return true;
    return false;
}
/**
 * @brief iter the tokens of the line. it extract the first token to know
 * wich variable to set in Request's map.
 */
void RequestParser::lineParser(RequestParser *ref, std::string &requestLine)
{
    requestLine += "\n";
    std::istringstream  iss(requestLine);
    std::string         token;
    
    iss >> token;
    
    if(token == "GET" || token == "POST" || token == "DELETE" || token == "PUT" || token == "HEAD" )
        receivedLineParse(ref, iss, token);
    else if (token == "User-Agent:")
        LineParseConcat(ref, iss, token);
    else if (token == "Content-Type:")
    {
        contenTypeParse(ref, iss);
    }
    else if (token == "Host:" && iss >> token)
        ref->set("HTTP_HOST", token);
    else if (token == "Accept:" && iss >> token)
        ref->set("HTTP_ACCEPT", token);
    else if (token == "Accept-Language:" && iss >> token)
        ref->set("HTTP_ACCEPT_LANGUAGE", token);
    else if (token == "Accept-Encoding:" && iss >> token)
        ref->set("HTTP_ACCEPT_ENCODING", token);    
    else if (token == "Content-Length:" && iss >> token)
        ref->set("CONTENT_LENGTH", token);
    else if (token == "DNT:" && iss >> token)
        ref->set("HTTP_DNT", token);
    else if (token == "Connection:" && iss >> token)
        ref->set("HTTP_CONNECTION", token);
    else if (token == "Upgrade-Insecure-Requests:" && iss >> token)
        ref->set("HTTP_UPGRADE_INSECURE_REQUESTS", token);
    else if (token == "Sec-Fetch-Dest:" && iss >> token)
        ref->set("HTTP_SEC_FETCH_DEST", token);
    else if (token == "Sec-Fetch-Mode:" && iss >> token)
        ref->set("HTTP_SEC_FETCH_MODE", token);  
    else if (token == "Sec-Fetch-Site:" && iss >> token)
        ref->set("HTTP_SEC_FETCH_SITE", token);
    else if (token == "Priority:" && iss >> token)
        ref->set("HTTP_PRIORITY", token);
    else if (token == "Pragma:" && iss >> token)
        ref->set("HTTP_PRAGMA", token);
    else if (token == "Cache-Control:" && iss >> token)
        ref->set("HTTP_CACHE_CONTROL", token);
    else if (token == "Content-Disposition:")
        contentDispositionParse(ref, iss);

    //ADDED THIS FOR HAVING THE HEADER AND BODY SEPARATED
    if (_headerWatchDog == 0)
        _requestHeader += requestLine;
    
    if (isTheEnd(requestLine) && _headerWatchDog == 0)
    {
        _headerWatchDog = 1;
        return;
    }
    if (_headerWatchDog == 1)
    {
        _requestBody += requestLine;
    }
}

/**
 * @brief Method to load on istringstream object the full message of request (client) 
 * This message had read before on the conexion's FD. Iter line per line parsing the message.
 * @return int -1 any exception. 0 ok.
 */
void RequestParser::loadConfigFromRequest(const std::string requestMessage)
{
    std::istringstream  stream(requestMessage);     // Create a string stream from a string message
    std::string         line;                       // string to store a line
 
    if ( requestMessage.empty() )
        throw ServerException("loadConfigFromRequest: is empty: ", BAD_REQUEST);
        //throw std::runtime_error("loadConfigFromRequest: is empty");
        
    while (std::getline(stream, line))              //Extract line per line till reach EOF
    {  
		try{
            lineParser(this, line);                 //Parse line per line 
        }catch (const std::exception& e) {
             throw ServerException("loadConfigFromRequest: " + std::string(e.what()), BAD_REQUEST);
            // throw std::runtime_error("loadConfigFromRequest: lineParser: " + std::string(e.what()));}
        }  
    }
   // std::cout <<  BYELLOW "Request Header: " RESET << _requestHeader << std::endl;
    //std::cout <<  BYELLOW "Request Body: '" RESET << _requestBody <<"'"<< std::endl;
    
    //std::cout <<  BCYAN "Request Body Lenght: " RESET << _requestBody.size() << std::endl;
    if (_requestBody.size() > 2)
        _bodyWatchDog = 1;
}

/**
 * @brief setter of the private Request's map.  
 */
void RequestParser::set(std::string key, std::string value)
{
    _requestData[key] = value;
}

/**
 * @brief return a reference of the private Request's map then can be manipulate
 * @return std::map<std::string, std::string>& 
 */
std::map<std::string, std::string> &RequestParser::get(void)
{
    return(_requestData);
}

/**
 * @brief write on terminal all the content of the Request's map for debugging.
 */
void RequestParser::showConfig(void)
{
    typedef std::map<std::string, std::string>::iterator it_t;
    
    std::cout << GREEN <<  "\n  <----------- HTTP REQUEST RECEIVED" << RESET << std::endl;
    for (it_t it = this->get().begin(); it != this->get().end(); ++it)
        std::cout << GREEN << it->first << RESET << " = " << YELLOW << it->second << RESET << std::endl;
     std::cout << GREEN <<  "-------------------------\n" << RESET << std::endl;
}

RequestParser::RequestParser(RequestParser const &src)
{
    *this = src;
}

RequestParser &RequestParser::operator=(RequestParser const &src)
{
    if (this != &src)
    {
        this->_requestData = src._requestData;
    }
    return *this;
}

std::string RequestParser::getRequestHeader(void) const
{
    return _requestHeader;
}

std::string RequestParser::getRequestBody(void) const
{
    return _requestBody;
}

int RequestParser::getHeaderWatchDog(void) const
{
    return _headerWatchDog;
}

int RequestParser::getBodyWatchDog(void) const
{
    return _bodyWatchDog;
}

std::string RequestParser::getPostFileName(void) const
{
    return _postFileName;
}

std::string RequestParser::getBoundary(void) const
{
    return _boundary;
}

bool RequestParser::getMultiPart(void) const
{
    return _multiPart;
}
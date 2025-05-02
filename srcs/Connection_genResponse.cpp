/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection_genResponse.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcereced <vcereced@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/16 13:30:21 by vcereced          #+#    #+#             */
/*   Updated: 2024/10/22 17:14:18 by vcereced         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Connection.hpp"

std::string Connection::genHeaderHTTP(std::string bodyHTTP, std::string filePath)
{
    std::stringstream   buffer;
    std::string         mime_type;
    std::string         response_header;
    std::string         response_header_params;
    long                size;
    long                statusCode;

    mime_type = this->getMimeType(filePath);//para python ???????????????????
    statusCode = this->getStatusCode();
    size = bodyHTTP.size();
    //CONVERT size to string
    std::string sizeStr;
    std::stringstream ss;
    ss << size;
    ss >> sizeStr;
    //end convert size to string
    if (statusCode == OK)
        response_header  = "HTTP/1.1 200 OK\r\n";
    else if (statusCode == NO_CONTENT)
        response_header  = "HTTP/1.1 204 No Content\r\n";
    else if (statusCode == BAD_REQUEST)
        response_header  = "HTTP/1.1 400 Bad Request\r\n";
    else if (statusCode == FORBIDDEN)
        response_header  = "HTTP/1.1 403 Forbidden\r\n";
    else if (statusCode == NOT_FOUND)
        response_header  = "HTTP/1.1 404 Not Found\r\n";
    else if (statusCode == METHOD_NOT_ALLOWED)
        response_header  = "HTTP/1.1 405 Method Not Allowed\r\n";
    else if (statusCode == PAYLOAD_TOO_LARGE)
        response_header  = "HTTP/1.1 413 Payload To Large\r\n";
    else if (statusCode == INTERNAL_SERVER_ERROR)
        response_header  = "HTTP/1.1 500 Internal Server Error\r\n";

    response_header_params = "Content-Type: " + mime_type + "\r\nContent-Length: " + sizeStr + "\r\n\r\n";

    return response_header + response_header_params;
}

std::string Connection::getValidDefaultIndex(void)
{
    std::string path = this->getPath();

    std::vector<std::string>::iterator it = this->_location.index.begin();
    for (; it != this->_location.index.end(); it++)
    {
        path = this->getPath() + *it;
        if (access(path.c_str(), F_OK)  != -1)
        {
            this->_statusCode = OK;
            return *it;
        }
    }
    throw ServerException("getValidDefaultIndex: not any index found ", NOT_FOUND);
    return "";
}

std::string Connection::genPathDefaultIndex(void)
{
    std::string                 responseHTTP;
    std::string                 defaultPath;

    defaultPath = this->getPath();

    defaultPath += getValidDefaultIndex();

    return defaultPath;
}

std::string Connection::genBodyFile(std::string filePath)
{
    std::string         responseHTTP_body;
    std::ifstream       file;
    std::stringstream   buffer;

    try{
       openFile(filePath, file);
    } catch (const ServerException &e) {
        throw ServerException("genBodyFile: " + std::string(e.what()), e.getCode());
    }

    buffer << file.rdbuf();
    responseHTTP_body = buffer.str();
    
    this->_statusCode = OK;
    
    return responseHTTP_body;
}

bool Connection::isCgi(void)
{
    std::map<std::string, std::string>::iterator    it; 
    std::string                                     key;

    key = this->_format;
    it = this->_location.cgiMap.find(key);

    if (it != this->_location.cgiMap.end())
        return true;
    else
        return false;
}

std::string Connection::genBodyHTTP(std::string filePath, RequestParser &request)
{
    std::string         responseHTTP_body;
    
    if (this->isCgi())
        responseHTTP_body = genBodyCgi(filePath, request);
    else
        responseHTTP_body = genBodyFile(filePath);

    return responseHTTP_body;
}

std::string Connection::genResponseGET(RequestParser &request)
{
    std::vector<std::string>    defaultIndexs;
    std::string                 responseHTTP_header;
    std::string                 responseHTTP_body;
    bool                        autoIndex;

    defaultIndexs = this->getLocation().index;
    autoIndex = this->getLocation().autoIndex;

    if (this->_location.redirection.statusCode != 0) //1º Redirection
    {
        return genResponseRedirection();
    }
    else if (this->_fileName.empty() == false) //2º file.html
    {
        responseHTTP_body = genBodyHTTP(this->_path, request);
    }
    else if (defaultIndexs.empty() == false) //3º default index
    {
        this->_path = genPathDefaultIndex();
        responseHTTP_body = genBodyHTTP(this->_path, request);
    }
    else if (autoIndex == true) //4º autoindex
    {
       responseHTTP_body = genBodyAutoIndex(this->_path);
    }
    else
        throw ServerException("genResponse: not default index and autoindex defined ", INTERNAL_SERVER_ERROR);
        
    responseHTTP_header = genHeaderHTTP(responseHTTP_body, this->_path);

    return responseHTTP_header + responseHTTP_body;
}

/**
 * @brief This function is used to parse the body of a POST request with a multipart/form-data content type.
 * it returns the index of the first boundary found in the buffer.
 * @param buffer 
 * @param boundary 
 * @param start 
 * @return size_t 
 */
size_t findBoundary(std::vector<unsigned char>& buffer, std::vector<unsigned char>& boundary, size_t start) {
    
    size_t bufferSize = buffer.size();
    size_t boundarySize = boundary.size();
    if (boundarySize == 0)
        return bufferSize;
    for (size_t i = start; i <= bufferSize - boundarySize; ++i) {
        bool found = true;
        for (size_t j = 0; j < boundarySize; ++j) {
            if (buffer[i + j] != boundary[j]) {
                found = false;
                break;
            }
        }
        if (found) {
            return i;
        }
    }
    return bufferSize; // Return the size of the buffer if the boundary is not found
}

/**
 * @brief This function extracts from buffer the part of the request between start and end.
 * start and end are indexes indicating the range of the part to extract.
 * @param buffer 
 * @param start 
 * @param end 
 * @return std::vector<unsigned char> 
 */
std::vector<unsigned char> extractRealBody(const std::vector<unsigned char>& buffer, size_t start, size_t end) {
    if (start >= buffer.size() || end > buffer.size() || start >= end) 
    return std::vector<unsigned char>();;
    return std::vector<unsigned char>(buffer.begin() + start, buffer.begin() + end);
}


/**
 * @brief This function is used to find the end of the headers in a part of a multipart/form-data request.
 *  it returns the index of first character after the headers end(/r/n/r/n).
 * @param buffer 
 * @param start 
 * @return size_t 
 */
size_t findHeadersEnd(const std::vector<unsigned char>& buffer, size_t start) {
    const char* doubleCRLF = "\r\n\r\n";
    size_t crlfSize = strlen(doubleCRLF);
    size_t bufferSize = buffer.size();
    
    for (size_t i = start; i <= bufferSize - crlfSize; ++i) {
        bool found = true;
        for (size_t j = 0; j < crlfSize; ++j) {
            if (buffer[i + j] != static_cast<unsigned char>(doubleCRLF[j])) {
                found = false;
                break;
            }
        }
        if (found) {
            return i + crlfSize;
        }
    }
    return bufferSize; // Return the size of the buffer if the headers end is not found
}


void Connection::createFilePost(std::string fileName, std::vector<unsigned char>& binary_data) {

    if (fileName.empty())
        fileName = "default_file";
    std::string path = this->_path + fileName;
    fileName = path;
    std::ofstream file(fileName.c_str(), std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(binary_data.data()), binary_data.size());
        file.close();
        std::cout << "File saved as " << fileName << std::endl;
    } else {
        throw ServerException("createFilePost: Cannot upload: " + fileName, 500);
    }
}

void Connection::processPost()
{
    std::string     fileName;
    std::string     boundary;
    size_t          part_start;
    size_t          part_end;
    std::vector<unsigned char> binary_data;
    
    boundary = _requestConnection.getBoundary();
    fileName = _requestConnection.getPostFileName();

    if (_requestConnection.getMultiPart() == true)
    {
        std::vector<unsigned char> boundaryVect(boundary.begin(), boundary.end());
        part_start = findBoundary(_buffer2, boundaryVect, 0);
        
        if (part_start == _buffer2.size())
            throw ServerException("processPost: Boundary not found", 500);
        
        part_start += boundaryVect.size() + 2; // +2 for skipping the /r/n after the boundary line
        part_end = findBoundary(_buffer2, boundaryVect, part_start); // Find the end of the part(next boundary line)

        std::vector<unsigned char> part = extractRealBody(_buffer2, part_start, part_end);
        size_t headers_end = findHeadersEnd(part, 0);
        if (headers_end == part.size())
            throw ServerException("processPost: Headers not found", 500);
            
        binary_data = extractRealBody(part, headers_end, part.size() - 2); // -2 to exclude the last /r/n
        
    }
    else
    {
        std::cout << BBLUE "HERE AT POST PROCESS"  RESET<< std::endl;
        std::cout << BBLUE "BODY: "  RESET<< _requestConnection.getRequestBody() << std::endl;
        const std::string& requestBody = _requestConnection.getRequestBody();
        binary_data.clear();
        for (std::string::const_iterator it = requestBody.begin(); it != requestBody.end(); ++it)
        {
            binary_data.push_back(static_cast<unsigned char>(*it));
        }
    }
    createFilePost(fileName, binary_data); 
}


std::string Connection::genResponse(RequestParser &request)
{
    std::string method;
    std::string response;

    method = request.get()["REQUEST_METHOD"];
    if (method == "GET")
        return genResponseGET(request);
    else if (method == "POST")
        return genResponsePOST(request);
    else if (method == "DELETE")
       return genResponseDELETE();
    else
        throw ServerException("genResponse: method not configured: " + method, BAD_REQUEST);
}
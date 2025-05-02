/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 20:08:17 by dgarizad          #+#    #+#             */
/*   Updated: 2024/10/02 10:54:59 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP
# define CONNECTION_HPP
# include "webserv.hpp"
# include "VHost.hpp"
# include "RequestParser.hpp"
# include <sys/stat.h>
# include <exception>
# include <ctime>
# include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <filesystem>

class VHost;

class Connection : public VHost
{ 
    public:
        Connection();
        ~Connection();
        Connection(Connection const &src);
        Connection &operator=(Connection const &src);
        Connection &operator=(VHost &src);

        void readFromSocket(void);

        //GETTERS
        int         getClientSocket() const;
        std::string getBuffer() const;
		std::string getPath() const;
		int         getStatusCode() const;
        t_location  getLocation() const;
        std::string getFileName() const;
        std::string getValidDefaultIndex(void);
        t_location  getLocationVHost(std::string uriRequested);
        bool        getKeepAlive(void) const;
        RequestParser& getRequest(void);

        //SETTERS
        void        setClientData(int clientSocket, sockaddr_in clientAddr, socklen_t clientAddrSize, struct epoll_event ev,  long maxSize);
        void        setBuffer(std::string buffer);
        void        setStatusCode(int statusCode);
        void        setVarsEnviroment(RequestParser &request);


        //DETERMINATOR
        bool        methodCheck(std::string method);
        void        requestParse(RequestParser &request);
        void        requestCheck(RequestParser &request);
        int         setDefaultIndex(void);

        //SERVE PAGE
   
        std::string getMimeType(const std::string &path);
        void        serveErrorPage(std::string filePath);  
        std::string genBodyCgi(std::string filePath, RequestParser &request);
        std::string genBodyFile(std::string filePath);
        std::string genBodyHTTP(std::string filePath, RequestParser &request);
        std::string genBodyAutoIndex(std::string route);
        std::string genHeaderHTTP(std::string bodyHTTP, std::string filePath);
        std::string genResponse(RequestParser &request);
        std::string genPathDefaultIndex(void);
        std::string genResponseGET(RequestParser &request);
        std::string genResponsePOST(RequestParser &request);
        std::string genResponseDELETE(void);
        std::string genResponseRedirection(void);
        void        Response(int clientSocket, const char * response, size_t size);

        void               processRequest(RequestParser &request);

        //POST
        void        processPost();
        void        createFilePost(std::string fileName, std::vector<unsigned char>& binary_data);


        bool        isCgi(void);


    private:
        int                         _clientSocket;
        sockaddr_in                 _clientAddr;
        socklen_t                   _clientAddrSize;
        struct epoll_event          _ev;
        std::string                 _buffer; //maybe a char array is better
        t_location                  _location;
        std::string                 _queryString;
        std::string                 _fileName;
        std::string                 _path;
        std::string                 _pathInfo;
        std::string                 _format;
		int                         _statusCode;
        bool                        _keepAlive;
        bool                        _download;
        RequestParser               _requestConnection;
        long                        _clientMaxBodySize;
        //POST
        std::string                 _headerPost;
        std::string                 _body;
        std::string                 _boundary;
        std::string                 _postFileName;
        std::vector<unsigned char>           _buffer2;
        std::vector<unsigned char>           _buffer3;
};

#endif
// #include "../includes/Master.hpp"
// #include <sys/types.h>
// #include <fcntl.h>

// int Master::setSockets(std::set<int> &ports)
// {
//     std::set<int>::iterator it = ports.begin();
//     for (; it != ports.end(); ++it)
//     {
//         int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
//         if (serverSocket < 0)
//             ft_error("Error creating socket");
//         struct sockaddr_in serverAddr;
//         serverAddr.sin_family = AF_INET;
//         serverAddr.sin_port = htons(*it);
//         serverAddr.sin_addr.s_addr = INADDR_ANY;
//         // Set socket to non-blocking
//         fcntl(serverSocket, F_SETFL, O_NONBLOCK);
//         if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
//             ft_error("Error binding socket");
//         if (listen(serverSocket, 10) < 0)
//             ft_error("Error listening on socket");
//         _ListenSockets.push_back(serverSocket);
//         std::cout << GREEN << "Socket " << serverSocket << " listening on port " << *it << RESET << std::endl;
//     }
//     return (0);
// }

// int Master::startEventLoop()
// {
//     fd_set readfds;
//     int maxfd = 0;

//     while (42)
//     {
//         FD_ZERO(&readfds);

//         for (auto& socket : _ListenSockets)
//         {
//             FD_SET(socket, &readfds);
//             if (socket > maxfd)
//                 maxfd = socket;
//         }

//         int activity = select(maxfd + 1, &readfds, NULL, NULL, NULL);

//         if (activity < 0)
//             ft_error("Error in select");

//         for (auto& socket : _ListenSockets)
//         {
//             if (FD_ISSET(socket, &readfds))
//             {
//                 sockaddr_in clientAddr;
//                 socklen_t clientAddrSize = sizeof(clientAddr);
//                 int clientSocket = accept(socket, (struct sockaddr *)&clientAddr, &clientAddrSize);
//                 if (clientSocket < 0)
//                     ft_error("Error accepting connection");
//                 std::cout << GREEN << "Accepted connection on socket " << socket << " from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << RESET << std::endl;
//                 _clientSockets.push_back(clientSocket);
//             }
//         }

//         for (auto& socket : _clientSockets)
//         {
//             if (FD_ISSET(socket, &readfds))
//             {
//                 char buffer[1024];
//                 int bytesRead = read(socket, buffer, 1024);
//                 if (bytesRead < 0)
//                     ft_error("Error reading from socket");
//                 if (bytesRead == 0)
//                     ft_error("Client disconnected");
//                 else
//                 {
//                     buffer[bytesRead] = '\0';
//                     std::cout << "Received: " << buffer << std::endl;
//                     //SERVING the ./html/index.html file
//                     std::ifstream file("../html/index.html");
//                     std::stringstream bufferr;
//                     bufferr << file.rdbuf();
//                     std::string file_contents = bufferr.str();
//                     const char *response_headers = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
//                     send(socket, response_headers, strlen(response_headers), 0);
//                     send(socket, file_contents.c_str(), file_contents.size(), 0);
//                     close(socket);
//                     std::cout << GREEN "Response sent" RESET << std::endl;
//                 }
//             }
//         }
//     }
//     return (0);
// }
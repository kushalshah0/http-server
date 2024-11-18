#include "http_tcpServer.h"

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <fstream>
#include <cstring> // For std::strlen
#include <ctime>
#include <iomanip>
#include <unordered_map>

namespace
{
    const int BUFFER_SIZE = 30720;

    std::string currentDateTime()
    {
        std::time_t now = std::time(0);
        std::tm *tm_now = std::localtime(&now);

        std::ostringstream oss;
        oss << std::put_time(tm_now, "%Y-%m-%d %H:%M:%S"); // Format: YYYY-MM-DD HH:MM:SS
        return oss.str();
    }

    void log(const std::string &message, const std::string &ip = "")
    {
        std::string timestamp = currentDateTime();
        if (!ip.empty())
        {
            std::cout << "[" << timestamp << "] [IP: " << ip << "] " << message << std::endl;
        }
        else
        {
            std::cout << "[" << timestamp << "] " << message << std::endl;
        }
    }

    void exitWithError(const std::string &errorMessage)
    {
        log("ERROR: " + errorMessage);
        exit(1);
    }
}

namespace http
{

    TcpServer::TcpServer(std::string ip_address, int port) : m_ip_address(ip_address), m_port(port), m_socket(), m_new_socket(),
                                                             m_incomingMessage(),
                                                             m_socketAddress(), m_socketAddress_len(sizeof(m_socketAddress))
    {
        m_socketAddress.sin_family = AF_INET;
        m_socketAddress.sin_port = htons(m_port);
        m_socketAddress.sin_addr.s_addr = inet_addr(m_ip_address.c_str());

        if (startServer() != 0)
        {
            std::ostringstream ss;
            ss << "Failed to start server with PORT: " << ntohs(m_socketAddress.sin_port);
            log(ss.str());
        }
    }

    TcpServer::~TcpServer()
    {
        closeServer();
    }

    int TcpServer::startServer()
    {
        m_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (m_socket < 0)
        {
            exitWithError("Cannot create socket");
            return 1;
        }

        if (bind(m_socket, (sockaddr *)&m_socketAddress, m_socketAddress_len) < 0)
        {
            exitWithError("Cannot connect socket to address");
            return 1;
        }

        return 0;
    }

    void TcpServer::closeServer()
    {
        close(m_socket);
        close(m_new_socket);
        exit(0);
    }

    void TcpServer::startListen()
    {
        if (listen(m_socket, 20) < 0)
        {
            exitWithError("Socket listen failed");
        }

        std::ostringstream ss;
        ss << "Server Started on ADDRESS:  " << inet_ntoa(m_socketAddress.sin_addr) << ":" << ntohs(m_socketAddress.sin_port);
        log(ss.str());

        int bytesReceived;

        while (true)
        {
            // log("\n\n====== Waiting for a new connection ======\n\n");
            acceptConnection(m_new_socket);

            char buffer[BUFFER_SIZE] = {0};
            bytesReceived = read(m_new_socket, buffer, BUFFER_SIZE);
            if (bytesReceived < 0)
            {
                log("Failed to read bytes from client socket connection");
                close(m_new_socket);
                continue;
            }

            std::ostringstream ss;
            std::string request(buffer, bytesReceived);
            size_t firstSpace = request.find(' ');
            size_t secondSpace = request.find(' ', firstSpace + 1);
            std::string methodAndPath = request.substr(0, secondSpace);
            log(methodAndPath, inet_ntoa(m_socketAddress.sin_addr));
            std::string response = buildResponse(buffer);
            sendResponse(response);

            close(m_new_socket);
        }
    }

    void TcpServer::acceptConnection(int &new_socket)
    {
        new_socket = accept(m_socket, (sockaddr *)&m_socketAddress, &m_socketAddress_len);
        if (new_socket < 0)
        {
            std::ostringstream ss;
            ss << "Server failed to accept incoming connection from ADDRESS: " << inet_ntoa(m_socketAddress.sin_addr) << "; PORT: " << ntohs(m_socketAddress.sin_port);
            exitWithError(ss.str());

            std::string client_ip = inet_ntoa(m_socketAddress.sin_addr);
            log("Accepted connection from client", client_ip);
        }
    }

    std::string TcpServer::buildResponse(const char *request)
    {
        std::string req(request);
        std::string method = req.substr(0, req.find(' '));

        std::string path = req.substr(req.find(' ') + 1, req.find(' ', req.find(' ') + 1) - req.find(' ') - 1);

        // Remove leading '/'
        if (!path.empty() && path[0] == '/')
        {
            path = path.substr(1);
        }

        std::ostringstream response;

        if (method == "GET")
        {
            // Create an unordered_map to map paths to file paths and content types
            std::unordered_map<std::string, std::pair<std::string, std::string>> fileMap = {
                {"", {"assets/index.html", "text/html"}},
                {"index.html", {"assets/index.html", "text/html"}},
                {"style.css", {"assets/style.css", "text/css"}},
                {"script.js", {"assets/script.js", "application/javascript"}},
                {"favicon.png", {"assets/favicon.png", "image/x-icon"}},
                {"assets/slides/0.jpeg", {"assets/slides/0.jpeg", "image/jpeg"}},
                {"assets/slides/1.jpeg", {"assets/slides/1.jpeg", "image/jpeg"}},
                {"assets/slides/2.jpeg", {"assets/slides/2.jpeg", "image/jpeg"}},
                {"assets/slides/3.jpeg", {"assets/slides/3.jpeg", "image/jpeg"}},
                {"assets/slides/4.jpeg", {"assets/slides/4.jpeg", "image/jpeg"}},
                {"assets/slides/5.jpeg", {"assets/slides/5.jpeg", "image/jpeg"}},
                {"assets/slides/6.jpeg", {"assets/slides/6.jpeg", "image/jpeg"}},
                {"assets/slides/7.jpeg", {"assets/slides/7.jpeg", "image/jpeg"}},
                {"assets/slides/8.jpeg", {"assets/slides/8.jpeg", "image/jpeg"}},
                {"assets/slides/9.jpeg", {"assets/slides/9.jpeg", "image/jpeg"}},
                {"assets/slides/10.jpeg", {"assets/slides/10.jpeg", "image/jpeg"}}};

            auto it = fileMap.find(path);
            if (it != fileMap.end())
            {
                const std::string &filePath = it->second.first;
                const std::string &contentType = it->second.second;

                std::ifstream file(filePath.c_str());
                if (file)
                {
                    std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                    response << "HTTP/1.1 200 OK\r\n"
                             << "Content-Type: " << contentType << "\r\n"
                             << "Content-Length: " << fileContent.size() << "\r\n\r\n"
                             << fileContent;
                }
                else
                {
                    response << "HTTP/1.1 404 Not Found\r\n"
                             << "Content-Type: text/html\r\n"
                             << "Content-Length: 60\r\n\r\n"
                             << "<html><body><h1>404 Not Found</h1></body></html>";
                }
            }
            else
            {
                response << "HTTP/1.1 404 Not Found\r\n"
                         << "Content-Type: text/html\r\n"
                         << "Content-Length: 60\r\n\r\n"
                         << "<html><body><h1>404 Not Found</h1></body></html>";
            }
        }
        else
        {
            response << "HTTP/1.1 405 Method Not Allowed\r\n"
                     << "Content-Type: text/html\r\n"
                     << "Content-Length: 60\r\n\r\n"
                     << "<html><body><h1>405 Method Not Allowed</h1></body></html>";
        }

        return response.str();
    }

    void TcpServer::sendResponse(const std::string &responseMessage)
    {
        long bytesSent = write(m_new_socket, responseMessage.c_str(), responseMessage.size());

        if (bytesSent == responseMessage.size())
        {
            log("Servered Successfully", inet_ntoa(m_socketAddress.sin_addr));
        }
        else
        {
            log("Error sending response to client", inet_ntoa(m_socketAddress.sin_addr));
        }
    }
} //
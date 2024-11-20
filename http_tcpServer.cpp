#include "http_tcpServer.h"

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <unordered_map>

class User
{
private:
    std::unordered_map<std::string, std::string> m_data;

public:
    User(const std::string &userId)
    {
        // Populate user data (mocked here)
        m_data["userId"] = userId;
        m_data["name"] = "John Doe";
        m_data["email"] = "johndoe@example.com";
        m_data["age"] = "30";
        m_data["gender"] = "male";
        m_data["address"] = "123 Main St, Anytown, USA";
        m_data["phone"] = "123-456-7890";
        m_data["occupation"] = "Software Engineer";
        m_data["company"] = "Tech Corp";
        m_data["hobbies"] = "[\"reading\", \"coding\", \"gaming\"]";
        m_data["website"] = "https://example.com";
    }

    std::string serialize() const
    {
        std::ostringstream oss;
        oss << "{";
        for (auto it = m_data.begin(); it != m_data.end(); ++it)
        {
            if (it != m_data.begin())
                oss << ",";
            oss << "\"" << it->first << "\": \"" << it->second << "\"";
        }
        oss << "}";
        return oss.str();
    }

    static User deserialize(const std::string &data)
    {
        // This is just a stub for deserialization; implement based on your input structure
        size_t idPos = data.find("\"userId\":");
        std::string userId = data.substr(idPos + 10, data.find("\"", idPos + 10) - (idPos + 10));
        return User(userId);
    }
};

namespace
{
    const int BUFFER_SIZE = 30720;

    std::string currentDateTime()
    {
        std::time_t now = std::time(0);
        std::tm *tm_now = std::localtime(&now);

        std::ostringstream oss;
        oss << std::put_time(tm_now, "%Y-%m-%d %H:%M:%S");
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
    TcpServer::TcpServer(std::string ip_address, int port) : m_ip_address(ip_address), m_port(port)
    {
        m_socketAddress.sin_family = AF_INET;
        m_socketAddress.sin_port = htons(m_port);
        m_socketAddress.sin_addr.s_addr = inet_addr(m_ip_address.c_str());

        if (startServer() != 0)
        {
            log("Failed to start server with PORT: " + std::to_string(ntohs(m_socketAddress.sin_port)));
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

        if (bind(m_socket, (sockaddr *)&m_socketAddress, sizeof(m_socketAddress)) < 0)
        {
            exitWithError("Cannot connect socket to address");
            return 1;
        }

        return 0;
    }

    void TcpServer::closeServer()
    {
        close(m_socket);
        exit(0);
    }

    void TcpServer::startListen()
    {
        if (listen(m_socket, 20) < 0)
        {
            exitWithError("Socket listen failed");
        }

        log("Server Started on ADDRESS: " + std::string(inet_ntoa(m_socketAddress.sin_addr)) + ":" + std::to_string(ntohs(m_socketAddress.sin_port)));

        while (true)
        {
            acceptConnection(m_new_socket);
            char buffer[BUFFER_SIZE] = {0};
            int bytesReceived = read(m_new_socket, buffer, BUFFER_SIZE);
            if (bytesReceived < 0)
            {
                log("Failed to read bytes from client socket connection");
                close(m_new_socket);
                continue;
            }

            std::string request(buffer, bytesReceived);
            size_t firstSpace = request.find(' ');
            size_t secondSpace = request.find(' ', firstSpace + 1);
            std::string methodAndPath = request.substr(0, secondSpace);
            log(methodAndPath, inet_ntoa(m_socketAddress.sin_addr));

            // Get response and status code
            auto responsePair = buildResponse(buffer);
            sendResponse(responsePair.first, responsePair.second);

            close(m_new_socket);
        }
    }

    void TcpServer::acceptConnection(int &new_socket)
    {
        new_socket = accept(m_socket, (sockaddr *)&m_socketAddress, (socklen_t *)&m_socketAddress_len);
        if (new_socket < 0)
        {
            exitWithError("Server failed to accept incoming connection");
        }
    }

    std::pair<std::string, int> TcpServer::buildResponse(const char *request)
    {
        std::string req(request);
        std::string method = req.substr(0, req.find(' '));
        std::string path = req.substr(req.find(' ') + 1, req.find(' ', req.find(' ') + 1) - req.find(' ') - 1);

        if (!path.empty() && path[0] == '/')
        {
            path = path.substr(1);
        }

        std::ostringstream response;
        int statusCode = 200; // Default to 200 OK
        try
        {
            if (method == "GET")
            {
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

                    std::ifstream file(filePath);
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
                        statusCode = 404;
                        response << "HTTP/1.1 404 Not Found\r\n"
                                 << "Content-Type: text/html\r\n"
                                 << "Content-Length: 60\r\n\r\n"
                                 << "<html><body><h1>404 Not Found</h1></body></html>";
                    }
                }
                else
                {
                    statusCode = 404;
                    response << "HTTP/1.1 404 Not Found\r\n"
                             << "Content-Type: text/html\r\n"
                             << "Content-Length: 60\r\n\r\n"
                             << "<html><body><h1>404 Not Found</h1></body></html>";
                }
            }
         if (method == "POST" && path == "user")
        {
            // Extract the payload
           size_t payloadStart = req.find("\r\n\r\n") + 4;
    std::string payload = req.substr(payloadStart);

    // Parse the payload (key=value&key=value format)
    std::unordered_map<std::string, std::string> formData;
    std::stringstream payloadStream(payload);
    std::string keyValuePair;
    while (std::getline(payloadStream, keyValuePair, '&')) {
        size_t equalsPos = keyValuePair.find('=');
        if (equalsPos != std::string::npos) {
            std::string key = keyValuePair.substr(0, equalsPos);
            std::string value = keyValuePair.substr(equalsPos + 1);
            formData[key] = value;
        }
    }

    // Check if userId exists in the form data
    if (formData.find("userId") != formData.end()) {
        User user(formData["userId"]); // Create a User object using the userId

        // Serialize User object to JSON
        std::string userJson = user.serialize();

        response << "HTTP/1.1 200 OK\r\n"
                 << "Content-Type: application/json\r\n"
                 << "Content-Length: " << userJson.size() << "\r\n\r\n"
                 << userJson;
    } else {
        statusCode = 400;
        response << "HTTP/1.1 400 Bad Request\r\n"
                 << "Content-Type: text/html\r\n"
                 << "Content-Length: 60\r\n\r\n"
                 << "<html><body><h1>400 Bad Request</h1></body></html>";
    }
        }
            else
            {
                statusCode = 405;
                response << "HTTP/1.1 405 Method Not Allowed\r\n"
                         << "Content-Type: text/html\r\n"
                         << "Content-Length: 60\r\n\r\n"
                         << "<html><body><h1>405 Method Not Allowed</h1></body></html>";
            }
        }
        catch (const std::exception &e)
        {
            response << "HTTP/1.1 500 Internal Server Error\r\n"
                     << "Content-Type: text/html\r\n"
                     << "Content-Length: 60\r\n\r\n"
                     << "<html><body><h1>500 Internal Server Error</h1></body></html>";
        }

        return {response.str(), statusCode};
    }

    void TcpServer::sendResponse(const std::string &responseMessage, int statusCode)
    {
        long bytesSent = write(m_new_socket, responseMessage.c_str(), responseMessage.size());

        if (statusCode == 200 && bytesSent == responseMessage.size())
        {
            log("[200] Served Successfully", inet_ntoa(m_socketAddress.sin_addr));
        }
        else if (statusCode == 404)
        {
            log("[404] File Not Found", inet_ntoa(m_socketAddress.sin_addr));
        }
        else if (statusCode == 405)
        {
            log("[405] Method Not Allowed", inet_ntoa(m_socketAddress.sin_addr));
        }
        else if (statusCode == 500)
        {
            log("[500] Internal Server Error", inet_ntoa(m_socketAddress.sin_addr));
        }
        else
        {
            log("Error sending response to client", inet_ntoa(m_socketAddress.sin_addr));
        }
    }
} // namespace http
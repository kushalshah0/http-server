FROM gcc:8

COPY . /usr/src/http-server
WORKDIR /usr/src/http-server

RUN mkdir -p build/ && cd build/ && rm -rf * 
RUN cd build/ && g++ -I.. -o server ../server.cpp ../http_tcpServer.cpp -lstdc++ -std=c++11

EXPOSE 8080
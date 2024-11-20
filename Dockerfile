FROM gcc:8

COPY . /usr/src/http-server
WORKDIR /usr/src/http-server

RUN mkdir -p build/ && cd build/ && rm -rf * 
RUN cd build/ && g++ -I.. -o server ../server.cpp ../http_tcpServer.cpp -lstdc++ -std=c++11

#change this to whatever port you choose to run your server on
EXPOSE 8080
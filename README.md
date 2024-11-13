<div id="top"></div>

<!-- PROJECT LOGO -->
<br />
<div align="center">
<!--   <a href="https://github.com/othneildrew/Best-README-Template">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a> -->

  <h1 align="center">HTTP SERVER</h1>

  <p align="center">
    A basic implementation of an HTTP server done in C++ 🙂.
    <br />
    <a href="https://github.com/kushalshah0/http-server#readme"><strong>Explore the docs »</strong></a>
    <br />
    <br />
<!--     <a href="https://github.com/othneildrew/Best-README-Template">View Demo</a> -->
<!--     · -->
    <a href="https://github.com/kushalshah0/http-server/issues">Report Bug</a>
    ·
    <a href="https://github.com/kushalshah0/http-server/issues">Request Feature</a>
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#building-and-running">Building and Running</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#contributing">Contributing</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

<!-- [![Product Name Screen Shot][product-screenshot]](https://example.com) -->

This is basically a mini project showing one of the major ways in which HTTP servers are implemented. In summary, it uses a TCP socket to listen for incoming requests and sends back a basic HTTP response based on standards set by the [RFC documents](https://www.ietf.org/standards/rfcs/).

As previously mentioned, this implementation of an HTTP web server basically creates a TCP socket which:
* listens for incoming requests on a particular [socket address](https://www.ibm.com/docs/en/aix/7.1?topic=addresses-socket-in-tcpip) (which is mostly a combination of an IP address and a network port)
* synchronously processes network connections from a queue of threads built up from the listening
* reads the message sent by a client over the network connection and sends some dummy HTTP response.

Of course, there are many many many ways in which this code can be improved and/or extended, so this is an **FYI** saying that this is mainly for educational purposes. Do not go off trying to build your own server from scratch for use within production-level projects. Asides from the obvious "try not to re-invent the wheel rhethoric", chances are (with almost a certainty) you will end up with very serious security-related problems.

<p align="right">(<a href="#top">back to top</a>)</p>



### Built With


* [C++](http://www.cplusplus.org/)
* [Docker](https://www.docker.com/)
* [Cmake](https://cmake.org/)

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- GETTING STARTED -->
## Getting Started

This is a pretty simple project with just one requirement: 
* Docker (unless you want to build it manually, then you'd be needing C/C++ compiler).

### Prerequisites

You will need to have docker setup on your computer, which can be done fairly easily by following the steps on their [official documentation](https://docs.docker.com/get-docker/). The version run with docker is the linux version of the HTTP server implementation.


### Building and Running

With docker fully setup on your system:

1. navigate to your desired project directory
2. Clone the repo

   ```sh
   git clone https://github.com/kushalshah0/http-server.git
   ```
   (if you do not have git installed, take a brief detour and follow these [instructions](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git)).
3. navigate to the project folder *__http-server/__* and build the docker image

   ```sh
   sudo docker-compose build
   ```
4. start the server
   ```sh
   sudo docker-compose up
   ```

When you're done with the server and need it shut down:

1. open up a second terminal window and navigate to the project folder

2. bring down the docker container

   ```sh
   sudo docker-compose down
   ```  
 **NOTE:** if you make any changes to the code, you will have to re-compile as well by simply running **step 3** again.
 </br></br>
   
<p align="right">(<a href="#top">back to top</a>)</p>

<!-- USAGE EXAMPLES -->
## Usage

Once your server is up and running with `docker-compose up`, you can pop into any web browser and navigate to `localhost:8080/` to see the default HTTP response sent from the server. There will be an article coming soon where I'll talk more indept about the development of this HTTP server.

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#top">back to top</a>)</p>
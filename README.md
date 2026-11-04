# C++ Web Scraper

## Overview

This project is a web scraper being developed from scratch using C++. Its goal is to establish network connections with web servers, send HTTP requests, receive server responses, and extract useful information from web pages.

The project focuses on understanding how web communication works at a lower level rather than relying entirely on high-level web scraping libraries.

As a software engineer, I'm using this project to strengthen my understanding of C++ network programming, TCP socket communication, HTTP protocols, and secure communication using OpenSSL and TLS.

The long-term goal is to build a functional web scraper with a modular architecture that handles network communication, secure data transmission, HTTP responses, and HTML processing.

### Project Goals

- Implement TCP socket communication using Windows Winsock.
- Establish secure HTTPS connections using OpenSSL and TLS.
- Understand HTTP request and response handling.
- Implement mechanisms for receiving and processing server responses.
- Extract useful information from HTML documents.
- Develop a modular and maintainable C++ application.

### Software Demo Video

[Watch the Project Introduction and Development Environment Setup](https://youtu.be/1B1WoyycN4s?is=a0On4GWjLg2L4ij2)

## Development Environment

The project is developed using:

- **IDE:** Microsoft Visual Studio Community
- **Build System:** CMake
- **Dependency Manager:** Microsoft vcpkg
- **Networking API:** Windows Sockets (Winsock)
- **Security Library:** OpenSSL
- **Version Control:** Git and GitHub

### Programming Language

**C++**

C++ is used for implementing networking functionality, managing socket connections, handling HTTP communication, and processing received data.

The project also uses OpenSSL to implement TLS-based secure communication, protecting data exchanged between the scraper and remote web servers.

## Useful Websites

- [Microsoft Winsock Documentation](https://learn.microsoft.com/en-us/windows/win32/winsock/windows-sockets-start-page-2)
- [OpenSSL Documentation](https://docs.openssl.org/)
- [CMake Documentation](https://cmake.org/documentation/)
- [Microsoft vcpkg Documentation](https://learn.microsoft.com/en-us/vcpkg/)
- [HTTP RFC 9110](https://www.rfc-editor.org/rfc/rfc9110)
- [MDN HTTP Documentation](https://developer.mozilla.org/en-US/docs/Web/HTTP)


// WebScraper.cpp : Defines the entry point for the application.
//

//#include "WebScraper.h"
//#include "MySocket.cpp"
//#include <openssl/ssl.h>
//#include <openssl/err.h>

#include <iostream>

using namespace std;

int main() {
	cout << "Hello World" << endl;
	return 0;
}

//int main()
//{
//	MySocket mySocket;
//
//	string url;
//	cout << "Enter URL: ";
//	cin >> url;
//
//	if (!mySocket.load()) {
//		std::cout << "Failed to load necessary socket at main\n";
//		return -1;
//	}
//	if (!mySocket.initialize()) {
//		std::cout << "Failed to Initialize socket at main\n";
//		return -1;
//	}
//
//	//SOCKET sock = mySocket.createSocket("192.168.0.1", 80);
//	SOCKET sock = mySocket.createSocket(url.c_str(), 80, true);
//
//	if (sock < 0) {
//		std::cout << "Failed to create socket at ma`in\n";
//		return -1;
//	}
//	std::cout << "Socket created successfully at main: " << sock << "\n";
//
//	if(!mySocket.connectToServer(sock)) {
//		std::cout << "Failed to connect to server." << std::endl;
//	}
//	
//	std::string requestData = "GET /index.html HTTP/1.1\r\n"
//		//"Host: example.com\r\n"
//		"Connection: close\r\n"
//		"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.3\r\n"
//		"Content-Type: test/html\r\n"
//		"Content-Length: 18\r\n"
//		"\r\n";
//
//	mySocket.Send_Ptr(sock, requestData.c_str(), requestData.size(), 0);
//
//	std::vector<char> data;
//	char buffer[4096];
//
//	mySocket.receiveData(sock, buffer, sizeof(buffer), &data);
//
//	std::string response(data.begin(), data.end());
//
//	cout << "\n\n" << response << endl;
//
//	std::cout << "Hello CMake." << std::endl;
//
//	mySocket.cleanup();
//	mySocket.CloseSocket_Ptr(sock);
//	return 0;
// }

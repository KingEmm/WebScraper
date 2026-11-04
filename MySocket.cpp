//#pragma once

//#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <vector>
//#include <openssl.h>

class MySocket {
private:
	typedef int (WSAAPI* WSAStartupFunc)(WORD wVersionRequested, LPWSADATA lpWSAData);
	typedef int (WSAAPI* WSACleanupFunc)(void);
	typedef int (WSAAPI* WSAGetLastErrorFunc)();
	typedef SOCKET(WSAAPI* SocketFunc)(int af, int type, int protocol);
	typedef int (WSAAPI* CloseSocketFunc)(SOCKET sock);
	typedef int (WSAAPI* ConnectFunc)(SOCKET s, const struct sockaddr* name, int namelen);
	typedef int (WSAAPI* SendFunc)(SOCKET s, const char* buf, int len, int flags);
	typedef int (WSAAPI* RecvFunc)(SOCKET s, const char* buf, int len, int flags);
	typedef int(WSAAPI* GetAddrInfoFunc)(const char* hostname, const char* port, const addrinfo* hints, addrinfo** serverAddr);
	typedef int (WSAAPI* GetNameInfoFunc)(const struct sockaddr* sa, int salen, char* host, int hostlen, char* serv, int servlen, int flags);
	typedef int (WSAAPI* Gai_strerrorFunc)(int code);
	typedef int (WSAAPI* FreeAddrInfoFunc)(addrinfo* ai);

	WSAStartupFunc WSAStartup_Ptr;
	WSACleanupFunc WSACleanup_Ptr;
	SocketFunc Socket_Ptr;
	ConnectFunc Connect_Ptr;
	GetAddrInfoFunc GetAddrInfo_Ptr;
	GetNameInfoFunc GetNameInfo_Ptr;
	Gai_strerrorFunc Gai_strerror_Ptr;
	WSAGetLastErrorFunc WSAGetLastError_Ptr;
	FreeAddrInfoFunc FreeAddrInfo_Ptr;

	addrinfo hints, *serverAddr;

public:
	SendFunc Send_Ptr;
	RecvFunc Recv_Ptr;
	CloseSocketFunc CloseSocket_Ptr;

	bool load(){
		HMODULE hModule = LoadLibraryA("ws2_32.dll");

		if (!hModule) {
			std::cout << "Failed to load ws2_32.dll" << std::endl;
			return false;
		}

		WSAStartup_Ptr = (WSAStartupFunc)GetProcAddress(hModule, "WSAStartup");
		WSACleanup_Ptr = (WSACleanupFunc)GetProcAddress(hModule, "WSACleanup");
		Socket_Ptr = (SocketFunc)GetProcAddress(hModule, "socket");
		CloseSocket_Ptr = (CloseSocketFunc)GetProcAddress(hModule, "closesocket");
		Connect_Ptr = (ConnectFunc)GetProcAddress(hModule, "connect");
		Send_Ptr = (SendFunc)GetProcAddress(hModule, "send");		
		Recv_Ptr = (RecvFunc)GetProcAddress(hModule, "recv");
		GetAddrInfo_Ptr = (GetAddrInfoFunc)GetProcAddress(hModule, "getaddrinfo");
		GetNameInfo_Ptr = (GetNameInfoFunc)GetProcAddress(hModule, "getnameinfo");
		//Gai_strerror_Ptr = (Gai_strerrorFunc)GetProcAddress(hModule, "gai_strerror");
		WSAGetLastError_Ptr = (WSAGetLastErrorFunc)GetProcAddress(hModule, "WSAGetLastError");
		FreeAddrInfo_Ptr = (FreeAddrInfoFunc)GetProcAddress(hModule, "freeaddrinfo");


		if (!WSAStartup_Ptr || !WSACleanup_Ptr || !Socket_Ptr || !CloseSocket_Ptr || !Connect_Ptr || !Send_Ptr || !Recv_Ptr || !GetAddrInfo_Ptr || !WSAGetLastError_Ptr) {
			std::cout << "Failed to load needed pointers\n";
			return false;
		}

		return true;
	}

	bool initialize() {
		WSAData wsaData;
		int startUp = WSAStartup_Ptr(MAKEWORD(2, 2), &wsaData);

		if (startUp != 0) {
			std::cerr << "WSAStartup failed with error: " << startUp << std::endl;
			return false;
		}

		std::cout << "Winsock initialized successfully" << std::endl;
		std::cout << "Winsock version: " << (int)wsaData.wVersion << std::endl;
		return true;
	}

	bool cleanup() {
		int cleanupResult = WSACleanup_Ptr();
		if (cleanupResult != 0) {
			std::cerr << "WSACleanup failed with error: " << cleanupResult << std::endl;
			return false;
		}
		std::cout << "Winsock cleaned up successfully" << std::endl;
		return true;
	}

	SOCKET createSocket(const char* hostname = "192.168.0.1", int port = 80, bool isAddr = false) {
		SOCKET sock=-1;
		int status;
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		if (isAddr) {
			status = GetAddrInfo_Ptr(hostname, std::to_string(port).c_str(), nullptr, &serverAddr);
			//std::cout << "ipaddress: " << (char  ddr->ai_addr << " port: " << port << " Socket created successfully: ";
			//std::cout << status <<" Getting addrinfo from by Hostname: " << hostname << std::endl;
		}
		else {
			status = GetAddrInfo_Ptr(hostname, std::to_string(port).c_str(), nullptr, &serverAddr);
			hints.ai_flags = AI_NUMERICHOST;
		}

		if (status != 0) {
			std::cout << "getaddrinfo failed with error: " 
				//<< Gai_strerror_Ptr(status) 
				<< gai_strerror(status)
				<< std::endl;
			return -1;
		}
		sock = Socket_Ptr(serverAddr->ai_family, serverAddr->ai_socktype, serverAddr->ai_protocol);

		//for (addrinfo* p = serverAddr; p != nullptr; p = p->ai_next) {
			char ip[NI_MAXHOST];

			status = GetNameInfo_Ptr(serverAddr->ai_addr, serverAddr->ai_addrlen,
				ip, sizeof(ip),
				nullptr, 0,
				NI_NUMERICHOST);

			if (status == 0)
				std::printf("%s\n", ip);
		//}

		if (sock == INVALID_SOCKET) {
			std::cerr << "Socket creation failed with error: " << WSAGetLastError_Ptr() << std::endl;
			return -1;
		}
		return sock;
	}

	bool connectToServer(SOCKET sock) {
		int connectResult = Connect_Ptr(sock, serverAddr->ai_addr, serverAddr->ai_addrlen);
		if (connectResult == SOCKET_ERROR) {
			std::cerr << "Connect failed with error: " << WSAGetLastError_Ptr() << std::endl;
			CloseSocket_Ptr(sock);
			FreeAddrInfo_Ptr(serverAddr);
			return false;
		}
		std::cout << "Connected to server successfully" << std::endl;
		//CloseSocket_Ptr(sock);
		return true;
	}

	int sendData(SOCKET sock, const char* data, int len) {
		int count = 0;
		while (count < len) {
			int bytesSent = Send_Ptr(sock, data + count, len - count, 0);
			if (bytesSent == SOCKET_ERROR) {
				std::cerr << "Send failed with error: " << WSAGetLastError_Ptr() << std::endl;
				return -1;
			}
			count += bytesSent;
		}
		return count;
	}

	int receiveData(SOCKET sock, char* buffer, int len, std::vector<char>* data) {
		int count = 0;
		int bytesReceived = 0;

		do {
			bytesReceived = Recv_Ptr(sock, buffer , len , 0);

			if (bytesReceived > 0) {
				data->insert(data->end(), buffer, buffer + bytesReceived);
			}
			else if (bytesReceived == 0) {
				break; // orderly connection close
			}
			else {
				if (errno == EINTR) {
					continue; // interrupted; try again
				}
				// Handle the socket error here.
				break;
			}
			if (bytesReceived == SOCKET_ERROR) {
				std::cerr << "Receive failed with error: " << WSAGetLastError_Ptr() << std::endl;
				return -1;
			}
			else if (bytesReceived == 0) {
				break; // Connection closed
			}
			count += bytesReceived;
		} while (bytesReceived > 0);
		return count;
	}	
};
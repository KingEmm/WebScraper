//#pragma once

//#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <vector>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509_vfy.h>

//#include <openssl.h>
//#pragma comment(lib, "ws2_32.lib")
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

	SOCKET createSocket(const char* hostname = "192.168.0.1", int port = 443, bool isAddr = false) {
		SOCKET sock = INVALID_SOCKET;
		int status;
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		if (isAddr) {
			status = GetAddrInfo_Ptr(hostname, 
				std::to_string(port).c_str(),
				nullptr, &serverAddr);
		}
		else {
			hints.ai_flags = AI_NUMERICHOST;
			status = GetAddrInfo_Ptr(hostname, std::to_string(port).c_str(), nullptr, &serverAddr);
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

	bool establishTLS(SOCKET socket, const std::string& hostname, SSL_CTX*& context, SSL*& ssl)
	{
		// 1. Create a TLS context
		context = SSL_CTX_new(TLS_client_method());

		if (!context) {
			ERR_print_errors_fp(stderr);
			return false;
		}

		// Require at least TLS 1.2
		if (SSL_CTX_set_min_proto_version(context, TLS1_2_VERSION) != 1) {
			ERR_print_errors_fp(stderr);
			std::cerr << "Failed to set minimum protocol version to TLS 1.2" << std::endl;
			SSL_CTX_free(context);
			context = nullptr;
			return false;
		}

		// 2. Enable server certificate verification
		SSL_CTX_set_verify(context, SSL_VERIFY_PEER, nullptr);

		//Load trusted CA certificates
		//if (SSL_CTX_set_default_verify_paths(context) != 1) {
		//	ERR_print_errors_fp(stderr);
		//	std::cerr << "Failed to load default CA certificates" << std::endl;
		//	SSL_CTX_free(context);
		//	context = nullptr;
		//	return false;
		//}

		if (SSL_CTX_load_verify_locations(context, "certs/cacert-2026-11-05.pem", nullptr) != 1)
		{
			ERR_print_errors_fp(stderr);
			std::cerr << "Failed to load CA certificate bundle\n";

			SSL_CTX_free(context);
			context = nullptr;

			return false;
		}

		// 3. Create an SSL connection object
		ssl = SSL_new(context);

		if (!ssl) {
			ERR_print_errors_fp(stderr);
			std::cerr << "Failed to create SSL object" << std::endl;
			SSL_CTX_free(context);
			context = nullptr;
			return false;
		}

		// 4. Configure SNI
		if (SSL_set_tlsext_host_name(ssl, hostname.c_str()) != 1) {
			ERR_print_errors_fp(stderr);
			std::cerr << "Failed to set SNI hostname" << std::endl;
			SSL_free(ssl);
			SSL_CTX_free(context);
			ssl = nullptr;
			context = nullptr;
			return false;
		}

		// Verify that the certificate matches the hostname
		if (SSL_set1_host(ssl, hostname.c_str()) != 1) {
			ERR_print_errors_fp(stderr);
			std::cerr << "Failed to set hostname for certificate verification" << std::endl;
			SSL_free(ssl);
			SSL_CTX_free(context);
			ssl = nullptr;
			context = nullptr;
			return false;
		}

		// 5. Attach existing Winsock socket
		if (SSL_set_fd(ssl, static_cast<int>(socket)) != 1) {
			ERR_print_errors_fp(stderr);
			SSL_free(ssl);
			SSL_CTX_free(context);
			std::cerr << "Failed to attach socket to SSL object" << std::endl;
			ssl = nullptr;
			context = nullptr;
			return false;
		}

		// 6. Perform TLS handshake
		if (SSL_connect(ssl) != 1) {
			ERR_print_errors_fp(stderr);
			SSL_free(ssl);
			SSL_CTX_free(context);
			std::cerr << "TLS handshake failed" << std::endl;
			ssl = nullptr;
			context = nullptr;
			return false;
		}

		// Confirm certificate verification succeeded
		if (SSL_get_verify_result(ssl) != X509_V_OK) {
			std::cerr << "Server certificate verification failed\n";
			std::cerr << "Certificate verification error: " << SSL_get_verify_result(ssl) << std::endl;
			SSL_free(ssl);
			SSL_CTX_free(context);
			ssl = nullptr;
			context = nullptr;
			return false;
		}

		std::cout << "TLS connection established!\n";
		std::cout << "TLS version: " << SSL_get_version(ssl) << '\n';

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

	//int receiveData(SOCKET sock, char* buffer, int len, std::vector<char> *data) {
	//	int count = 0;
	//	int bytesReceived = 0;

	//	do {
	//		bytesReceived = Recv_Ptr(sock, buffer , len , 0);

	//		if (bytesReceived > 0) {
	//			data->insert(data->end(), buffer, buffer + bytesReceived);
	//		}
	//		else if (bytesReceived == 0) {
	//			break; // orderly connection close
	//		}
	//		else {
	//			if (errno == EINTR) {
	//				continue; // interrupted; try again
	//			}
	//			// Handle the socket error here.
	//			break;
	//		}
	//		if (bytesReceived == SOCKET_ERROR) {
	//			std::cerr << "Receive failed with error: " << WSAGetLastError_Ptr() << std::endl;
	//			return -1;
	//		}
	//		else if (bytesReceived == 0) {
	//			break; // Connection closed
	//		}
	//		count += bytesReceived;
	//	} while (bytesReceived > 0);
	//	return count;
	//}	

	int receiveData(SOCKET sock, char *buffer, size_t len, std::vector<char>* data) {
		int count = 0;
		int bytesReceived = 0;

		do {
			bytesReceived = Recv_Ptr(sock, buffer, len, 0);

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

	bool sendTLSData(SSL* ssl, const std::string& data)
	{
		size_t totalSent = 0;

		while (totalSent < data.size()) {
			size_t bytesSent = 0;

			int result = SSL_write_ex(
				ssl,
				data.data() + totalSent,
				data.size() - totalSent,
				&bytesSent
			);

			if (result != 1) {
				int error = SSL_get_error(ssl, result);

				std::cerr << "SSL_write_ex failed: "
					<< error << '\n';

				ERR_print_errors_fp(stderr);
				return false;
			}

			totalSent += bytesSent;
		}

		return true;
	}

	bool receiveTLSData(SSL* ssl, std::string* response)
	{
		char buffer[4096];

		while (true) {
			size_t bytesRead = 0;

			int result = SSL_read_ex(ssl, buffer, sizeof(buffer), &bytesRead);

			if (result == 1) {
				response->append(buffer, bytesRead);
				continue;
			}

			int error = SSL_get_error(ssl, result);

			if (error == SSL_ERROR_ZERO_RETURN) {
				std::cout << "Server sent TLS close_notify" << std::endl;
				break;
			}

			if (error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_WRITE) {
				std::cout << "Retry when the socket is ready" << std::endl;
				continue;
			}

				//std::wcerr << WSAGetLastError() << "TLS connection closed by server." << std::end;
			std::cerr << "TLS receive failed: " << error << '\n';
			ERR_print_errors_fp(stderr);
			return false;
		}

		return true;
	}

};
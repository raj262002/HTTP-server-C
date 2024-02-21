#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include"stdafx.h"
#include<iostream>
#include<WS2tcpip.h>
#include<WinSock2.h>
#include<tchar.h>

#define PORT 800
#define BUFFER_SIZE 1024

int main() {
	char buff[BUFFER_SIZE];
	char resp[] = "HTTP/1.0 200 OK\r\n"
		"Server: http-cpp\r\n"
		"Content -type: text/html\r\n\r\n"
		"<html>Hello SRK</html>\r\n";

	// load DLL
	WSADATA wsaData;
	int wsaerr;
	WORD wVersionRequested = MAKEWORD(2, 2);
	wsaerr = WSAStartup(wVersionRequested, &wsaData);
	if (wsaerr != 0) {
		std::cout << "The Winsock dll not Found" << std::endl;
		return 0;
	}
	else {
		std::cout << "The Winsock dll found" << std::endl;
		std::cout << "The status" << wsaData.szSystemStatus << std::endl;
	}

	//socket creation
	SOCKET serverSocket = INVALID_SOCKET;
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET) {
		std::cout << "error at SOCKET : " << WSAGetLastError() << std::endl;
		WSACleanup();
	}
	else std::cout << "Socket creation SUCCESSFUL" << std::endl;

	//bind IP address and Port number to the socket
	sockaddr_in service;
	service.sin_family = AF_INET;
	InetPton(AF_INET, _T("127.0.0.1"), &service.sin_addr.s_addr);
	service.sin_port = htons(PORT);

	//create client address
	sockaddr_in client_addr{};
		int client_addrlen = sizeof(client_addr);


	if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
		std::cout << "ERROR in binding" << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return -1;
	}
	else std::cout << "bind SUCCESSFUL" << std::endl;

	if (listen(serverSocket, 1) == SOCKET_ERROR) {
		std::cout << "ERROR while listening" << std::endl;
	}
	else {
		std::cout << "Server listening for CONNECTION"<<std::endl;
	}

	for (;;) {
		SOCKET acceptSocket = accept(serverSocket, NULL, NULL);
		if (acceptSocket == INVALID_SOCKET) {
			//std::cout << "ERROR in ACCEPT the CONNETION" << std::endl;
			continue;
		}
		std::cout << "CONNECTION ACCEPTED" << std::endl;

		//Get Client Address
		int sockn = getsockname(acceptSocket, (sockaddr*)&client_addr, (socklen_t*)&client_addrlen);
		if (sockn < 0) {
			std::cout << "ERROR no client Address" << std::endl;
			continue;
		}


		//read from the Socket
		int byteCount = recv(acceptSocket, buff, 1024, 0);
		if (byteCount < 0) {
			std::cout << "ERROR in READING the data" << std::endl;
			continue;
		}
		else {
			std::cout << "Recieved data" << buff << std::endl;
		}

		//Read the request
		char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
		printf("[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), method, version, uri);
		sscanf(buff, "%s %s %s\n", method, uri, version);

		//write to the socket
		int byteCountw = send(acceptSocket, resp, strlen(resp), 0);
		if (byteCountw < 0) {
			std::cout << "ERROR in Sending the Response" << std::endl;
			continue;
		}
		closesocket(acceptSocket);
		WSACleanup();
	}

	return 0;
}
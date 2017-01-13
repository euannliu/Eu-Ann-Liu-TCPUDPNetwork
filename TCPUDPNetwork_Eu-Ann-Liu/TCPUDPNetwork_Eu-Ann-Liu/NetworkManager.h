#pragma once
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <stdio.h>
#pragma comment(lib, "Ws2_32.lib")

enum NetworkType
{
	UDP,
	TCP
};

enum UserType
{
	Server,
	Client
};

class NetworkManager
{
public:
	NetworkManager(NetworkType nType, UserType uType, std::string ipAddress);
	~NetworkManager();
	bool SendData(char*);
	int RecvData(char*, int);
	void CloseConnection();

	//Server
	void Listen();
	void Bind(int port);
	void StartHosting(int port);

	//Client
	void ConnectToServer(int port);

protected:
	WSADATA m_wsaData;
	SOCKET m_socket; //your client's/server's socket
	SOCKET m_backup; //just in case
	sockaddr_in m_address; //Where you are going to connect to or your ip address depending if you're server or not
	sockaddr_in m_remote; //for udp, where to send back information. Is a set to store all the different clients that send to this server.
	socklen_t m_addrlen;
	std::string m_ipAddress;
	addrinfo *m_addrInfo;

	NetworkType m_nType;
	UserType m_uType;

	sockaddr_in m_remotes[8];
	int m_rcounter;
};



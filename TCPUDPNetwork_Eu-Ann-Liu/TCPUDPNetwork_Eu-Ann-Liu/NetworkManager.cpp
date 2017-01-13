#include "NetworkManager.h"

using namespace std;

NetworkManager::NetworkManager(NetworkType nType, UserType uType, string ipAddress)
{
	m_nType = nType;
	m_uType = uType;
	m_ipAddress = ipAddress;
	m_addrlen = sizeof(m_remote);
	m_rcounter = 0;

	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != NO_ERROR)
	{
		cerr << "Error: WSAStartup\n";
		system("pause");
		WSACleanup();
		exit(1);
	}

	if (m_nType == TCP){
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	}
	else if (m_nType == UDP){
		m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}

	if (m_socket == INVALID_SOCKET)
	{
		cerr << "Error: Creating Socket" << endl;
		system("pause");
		WSACleanup();
		exit(2);
	}

	m_backup = m_socket;
}

NetworkManager::~NetworkManager()
{
	//SendData("Eu-Ann disconnected.");
	WSACleanup();
	freeaddrinfo(m_addrInfo);
}

bool NetworkManager::SendData(char *buffer)
{
	if (m_nType == UDP) {
		
		if (m_uType == Server) 
		{
			/*
			if (sendto(m_socket, buffer, strlen(buffer), 0, (struct sockaddr *)&m_remote, sizeof(m_remote)) < 0) {
				perror("Error: sendto failed");
				exit(8);
			}
			*/
			for (int i = 0; i < m_rcounter; i++) {
				if (sendto(m_socket, buffer, strlen(buffer), 0, (struct sockaddr *)&m_remotes[i], sizeof(m_remote)) < 0) {
					perror("Error: sendto failed");
					exit(8);
				}
			}
		}
		else {
			if (sendto(m_socket, buffer, strlen(buffer), 0, (struct sockaddr *)&m_address, sizeof(m_address)) < 0) {
				perror("Error: sendto failed");
				exit(9);
			}
		}
	}

	//TCP
	else 
		send(m_socket, buffer, strlen(buffer), 0);

	return true;
}

int NetworkManager::RecvData(char *buffer, int size)
{
	if (m_nType == TCP) {
		int i = recv(m_socket, buffer, size, 0);
		buffer[i] = '\0';
		return i;
	}
	else {
		//return recvfrom(m_socket, buffer, size, 0, (struct sockaddr*)&m_remote, &m_addrlen);
		u_long nMode = 1; // 1 = NONBLOCKING
		if (ioctlsocket(m_socket, FIONBIO, &nMode) == SOCKET_ERROR)
		{
			closesocket(m_socket);
			WSACleanup();
			exit(10);
		}
		int i = recvfrom(m_socket, buffer, size, 0, (struct sockaddr*)&m_remote, &m_addrlen);

		if (i > 0 && m_rcounter < 8) {
			m_remotes[m_rcounter] = m_remote;
			m_rcounter++;
		}
		return i;
	}
}

void NetworkManager::CloseConnection()
{
	closesocket(m_socket);
	m_socket = m_backup;
}

void NetworkManager::StartHosting(int port)
{
	Bind(port);
	if (m_nType == TCP) { Listen(); }
}

void NetworkManager::Listen()
{
	cout<<"Waiting on client..."<<endl;
	if (listen(m_socket, 1) == SOCKET_ERROR)
	{
		cerr << "Error: Listening\n";
		system("pause");
		WSACleanup();
		exit(15);
	}

	SOCKET acceptSocket = accept(m_backup, NULL, NULL);
	while (acceptSocket == SOCKET_ERROR)
	{
		acceptSocket = accept(m_backup, NULL, NULL);
	}
	m_socket = acceptSocket;
	cout << "Connection Established!" << endl;
}

void NetworkManager::Bind(int port)
{
	unsigned long buffer;
	m_address.sin_family = AF_INET;
	InetPton(m_address.sin_family, m_ipAddress.c_str(), &buffer);
	m_address.sin_addr.s_addr = buffer;
	m_address.sin_port = htons(port);

	cout << "Binding to Port " << port << endl;

	if (bind(m_socket, (SOCKADDR*)&m_address, sizeof(m_address)) == SOCKET_ERROR)
	{
		cerr << "ServerSocket: Failed to connect\n";
		system("pause");
		WSACleanup();
		exit(3);
	}

	cout << "Port bound" << endl;
}

void NetworkManager::ConnectToServer(int port)
{
	if (m_nType == UDP) {
		cout << "Enter IP Address to send to: ";
		cin >> m_ipAddress;
	}

	unsigned long buffer;
	m_address.sin_family = AF_INET;
	InetPton(m_address.sin_family, m_ipAddress.c_str(), &buffer);
	m_address.sin_addr.s_addr = buffer;
	m_address.sin_port = htons(port);

	if (m_nType == TCP) {
		cout << "Connecting..." << endl;
		if (connect(m_socket, (SOCKADDR*)&m_address, sizeof(m_address)) == SOCKET_ERROR)
		{
			cerr << "ClientSocket: Failed to connect\n";
			system("pause");
			WSACleanup();
			exit(6);
		}
		cout << "CONNECTED!!" << endl;
	}
}

#include "NetworkManager.h"
#include <time.h>
#define STRLEN 256

using namespace std;

NetworkType nType;
UserType uType;
string ipAddress = "0.0.0.0";
int PORT = 8888;


void WriteToFile(char* buffer)
{
	FILE *ptr_myfile;

	fopen_s(&ptr_myfile,"test.bin", "wb");
	if (!ptr_myfile)
	{
		printf("Unable to open file!");
		exit(11);
	}

	fwrite(&buffer, sizeof(buffer), 1, ptr_myfile);

	fclose(ptr_myfile);
	exit(0);
}

void ChatSystem_Server(NetworkManager *network)
{
	cout << "Hosting server... Please wait!" << endl;
	network->StartHosting(PORT);

	while (nType == TCP)
	{
		char sendMessage[STRLEN] = "";
		char recMessage[STRLEN] = "";

		cout << "\t--Receiving--" << endl;
		network->RecvData(recMessage, STRLEN);
		cout << "Recv > " << recMessage << endl;

		cin.ignore();
		cout << "Send > ";
		cin.get(sendMessage, STRLEN);

		network->SendData(sendMessage);
	}
	if (nType == UDP)
	{
		time_t timer;
		bool done = false;
		bool firstTime = true;
		while (!done)
		{
			char sendMessage[STRLEN] = "";
			char recMessage[STRLEN] = "";
			cout << "\t--Receiving--" << endl;
			while (true) {
				if (network->RecvData(recMessage, STRLEN) > 0)
				{
					cout << "Recv > " << recMessage << endl;
					firstTime = false;
					network->SendData("I have received your message.");
					break;
				}
				/*
				if (difftime(time(0), timer) > 5 && !firstTime) {
					cout << "Connection time-out. Packet dropped...?" << endl;
					done = true;
					break;
				}
				*/
			}
			/*
			cin.ignore();
			cout << "Send > ";
			cin.get(sendMessage, STRLEN);

			network->SendData(sendMessage);
			timer = time(0);
			*/
		}
	}

	network->CloseConnection();
	cin.ignore();
	cin.ignore();
}

void ChatSystem_Client(NetworkManager *network)
{
	if(nType == TCP) cout << "Attempting to establish connection..." << endl;

	network->ConnectToServer(PORT);

	while (nType == TCP){
		char sendMessage[STRLEN] = "";
		char recMessage[STRLEN] = "";

		cin.ignore();
		cout << "Send > ";
		cin.get(sendMessage, STRLEN);
		network->SendData(sendMessage);

		cout << "\t--Receiving--" << endl;
		network->RecvData(recMessage, STRLEN);
		cout << "Recv > " << recMessage << endl;
	}

	if (nType == UDP)
	{
		time_t timer;
		bool done = false;

		while (!done) {
			char sendMessage[STRLEN] = "";
			char recMessage[STRLEN] = "";
			cin.ignore();
			cout << "Send > ";
			cin.get(sendMessage, STRLEN);

			network->SendData(sendMessage);

			timer = time(0);
			cout << "\t--Receiving--" << endl;
			while (true) {
				if (network->RecvData(recMessage, STRLEN) > 0)
				{
					cout << "Recv > " << recMessage << endl;
					break;
				}

				if (difftime(time(0), timer) > 5) {
					cout << "Connection time-out. Packet dropped...?" << endl;
					done = true;
					break;
				}
			}
		}
	}
	

	network->CloseConnection();
	cin.ignore();
	cin.ignore();
}

int main()
{
	cout << "Welcome to Eu-Ann's Network" << endl;
	cout << "1) TCP Connection\n2) UDP Connection" << endl;
	cout << ">> ";
	int x;
	cin >> x;
	if (x != 1 && x != 2) { exit(0); }
	else if (x == 1) { nType = NetworkType::TCP; }
	else if (x == 2) { nType = NetworkType::UDP; }

	cout << "\n1) Host\n2) Join" << endl;
	cout << ">> ";
	cin >> x;
	if (x != 1 && x != 2) { exit(0); }
	else if (x == 1) { uType = UserType::Server; }
	else if (x == 2) { uType = UserType::Client; }

	cout << "Enter Port: ";
	cin >> PORT;
	if (nType == TCP) {
		cout << "Enter IP: ";
		cin >> ipAddress;
	}

	NetworkManager *network = new NetworkManager(nType, uType, ipAddress);

	if (uType == Server) { ChatSystem_Server(network); }
	else if (uType == Client) { ChatSystem_Client(network); }

	return 0;
}
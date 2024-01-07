#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <map>
#include <vector>
#include <thread>
#include <shared_mutex>


using namespace std;


#include "ChatRoom.h"
#include "Packet.h"


#define LISTEN_CLIENT_MAXIMUM 100


class Client
{
public:
	SOCKET socket;
	char* ip;
	string account, password;
	clientStatus status;

	Client()
	{
		socket = INVALID_SOCKET;
		ip = new char[INET_ADDRSTRLEN] { 0 };
		account = "";
		password = "";
		status = clientStatus::loggingIn;
	}
	Client(SOCKET socket, char* ip)
	{
		this->socket = socket;
		this->ip = ip;
		account = "";
		password = "";
		status = clientStatus::loggingIn;
	}
	Client(SOCKET socket, char* ip, string account, string password, clientStatus status)
	{
		this->socket = socket;
		this->ip = ip;
		this->account = account;
		this->password = password;
		this->status = status;
	}
	~Client()
	{
		closesocket(socket);
		delete ip;
	}
};
bool equal(Client c1, Client c2)
{
	return c1.socket == c2.socket;
}

class Server
{
public:
	static Server instance;
	SOCKET socket;
	vector<Client> clientList;
	vector<thread> clientThreadList;

private:
	map<string, string> accountAndPassword;

private:
	Server()
	{
		socket = INVALID_SOCKET;
		clientList = vector<Client>();
		clientThreadList = vector<thread>();

		accountAndPassword = map<string, string>();
	}

public:
	static Server& getInstance()
	{
		return instance;
	}

	void updateAccountAndPassword()
	{

	}

	bool checkAccountExist(string account)
	{
		return accountAndPassword.find(account) != accountAndPassword.end();
	}

	bool checkAccountAndPassword(string account, string password)
	{
		return checkAccountExist(account) && accountAndPassword[account] == password;
	}

	void closeClient(Client* client)
	{
		for (int i = 0; i < clientList.size(); i++)
		{
			if (equal(clientList[i], *client))
			{
				clientList.erase(clientList.begin() + i);
				clientThreadList[i].detach();
				clientThreadList.erase(clientThreadList.begin() + i);
				break;
			}
		}

		client->~Client();
	}
};
Server Server::instance = Server();
Server& serverInstance = Server::getInstance();


#pragma region ErrorPrint
void restartServerErrorPrint(string errorFunctionName)
{
	cerr << errorFunctionName << "() failed with error: " << WSAGetLastError() << endl;
	cerr << "Restarting..." << endl;
	WSACleanup();
	closesocket(serverInstance.socket);
	Sleep(RESTART_WAIT_MS);
}
void reconnectClientErrorPrint(string errorFunctionName, Client* client)
{
	cerr << "When processing " << client->account << "-" << client->ip << ", " << errorFunctionName << "() failed with error: " << WSAGetLastError() << endl;
	serverInstance.closeClient(client);
}
#pragma endregion


#pragma region Send&Receive
bool sendToClient(Client* client, ServerPacket packet)
{
	int hasSent = 0, nowSend = 0;
	do
	{
		nowSend = send(client->socket, packet.msgBuff + hasSent, packet.msgLen - hasSent, 0);
		if (nowSend == SOCKET_ERROR)
		{
			reconnectClientErrorPrint("send", client);
			return false;
		}
		hasSent += nowSend;
	} while (hasSent != packet.msgLen);

	return true;
}

bool receiveFromClient(SOCKET clientSocket, ClientPacket* packet)
{
	// id section
	int hasReceived = 0;
	char* buff = new char[2] { 0 };

	hasReceived = recv(clientSocket, buff, 1, 0);
	if (hasReceived == SOCKET_ERROR)
	{
		restartServerErrorPrint("recv");
		return false;
	}

	clientPacketId id = (clientPacketId)to_int(buff, 1);

	// total length section
	hasReceived = recv(clientSocket, buff, 2, 0);
	if (hasReceived == SOCKET_ERROR)
	{
		restartServerErrorPrint("recv");
		return false;
	}

	int msgLen = to_int(buff, 2);

	// msg section
	int nowReceive = 0;
	hasReceived = 0;
	char* msgBuff = new char[msgLen] { 0 };
	do
	{
		nowReceive = recv(clientSocket, msgBuff + hasReceived, msgLen - hasReceived, 0);
		if (nowReceive == SOCKET_ERROR)
		{
			restartServerErrorPrint("recv");
			return false;
		}
		hasReceived += nowReceive;
	} while (hasReceived < msgLen);

	packet->setData(id, msgBuff, msgLen);

	return true;
}
#pragma endregion


#pragma region Initialization
bool initializeWSA(WSADATA* wsaData)
{
	int error = WSAStartup(MAKEWORD(2, 2), wsaData);
	if (error != 0)
	{
		cerr << "WSAStartup() failed with error: " << error << endl;
		return false;
	}

	return true;
}

bool initializeSocket()
{
	serverInstance.socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverInstance.socket == INVALID_SOCKET)
	{
		cerr << "socket() failed with error: " << WSAGetLastError() << endl;
		WSACleanup();
		return false;
	}

	return true;
}

bool setSocketOption()
{
	int opt = 1;
	int return_val = setsockopt(serverInstance.socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
	if (serverInstance.socket == SOCKET_ERROR)
	{
		restartServerErrorPrint("setsockopt");
		return false;
	}

	return true;
}

bool bindSocket()
{
	struct sockaddr_in bindingAddr = { AF_INET, htons(SERVER_PORT), INADDR_ANY };
	memset(&bindingAddr.sin_zero, 0, 8);

	int return_value = bind(serverInstance.socket, (sockaddr*)&bindingAddr, sizeof(bindingAddr));
	if (return_value == SOCKET_ERROR)
	{
		restartServerErrorPrint("bind");
		return false;
	}

	return true;
}

bool listenSocket()
{
	int return_value = listen(serverInstance.socket, LISTEN_CLIENT_MAXIMUM);

	if (return_value == SOCKET_ERROR)
	{
		restartServerErrorPrint("listen");
		return false;
	}

	cout << "Started listening to local port..." << endl;
	return true;
}
#pragma endregion


bool login(Client* client)
{
	
	SendLoginDataPacket loginDataPacket;
	if (!receiveFromClient(client->socket, &loginDataPacket))
	{
		restartServerErrorPrint("receiveFromServer");
		return false;
	}

	if (!serverInstance.checkAccountAndPassword(client->account, client->password))
	{
		cout << client->ip << " failed to log in" << endl;
		return false;
	}

	cout << client->account << "-" << client->ip << " successfully logged in" << endl;
	return true;
}


void clientProcess(Client* client)
{
	while (true)
	{
		switch (client->status)
		{
		case clientStatus::loggingIn:
			if (login(client))
				client->status = clientStatus::chatRoomList;
			break;

		case clientStatus::chatRoomList:
			//if (selectChatRoom(clientSocket))
				client->status = clientStatus::chatRoom;
			break;

		case clientStatus::chatRoom:

			break;
		}
	}

}


int main()
{
	WSADATA wsaData;
	

	while (true)
	{
		if (!initializeWSA(&wsaData))
			continue;

		if (!initializeSocket())
			continue;

		if (!setSocketOption())
			continue;

		cout << "The socket is successfully initialized, socket id=" << serverInstance.socket << endl;

		if (!bindSocket())
			continue;

		if (!listenSocket())
			continue; 

		while (true)
		{
			SOCKET clientSocket;
			struct sockaddr_in clientAddr;
			int addrLen = sizeof(sockaddr_in);

			clientSocket = accept(serverInstance.socket, (sockaddr*)&clientAddr, &addrLen);

			if (clientSocket == INVALID_SOCKET)
				continue;

			cout << "Accept client socket : " << clientSocket << endl;

			char client_ip[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &clientAddr, client_ip, INET_ADDRSTRLEN);

			serverInstance.clientList.emplace_back(clientSocket, client_ip);
			serverInstance.clientThreadList.emplace_back(clientProcess, &(serverInstance.clientList.back()));
		}
	}

	return 0;
}
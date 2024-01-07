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
		status = clientStatus::logging_in;
	}
	Client(SOCKET socket, char* ip)
	{
		this->socket = socket;
		this->ip = ip;
		account = "";
		password = "";
		status = clientStatus::logging_in;
	}

	void setAccountAndPassword(string account, string password)
	{
		this->account = account;
		this->password = password;
	}
};
bool equal(Client& c1, Client& c2)
{
	return c1.socket == c2.socket;
}

class Server
{
public:
	static Server* instance;
	SOCKET socket;
	vector<Client> clientList;
	vector<thread> clientThreadList;
	vector<ChatRoom> chatRoomList;

private:
	map<string, string> accountAndPassword;

	shared_mutex accountAndPassword_mutex, chatRoomList_mutex;

private:
	Server()
	{
		socket = INVALID_SOCKET;
		clientList = vector<Client>();
		clientThreadList = vector<thread>();
		chatRoomList = vector<ChatRoom>();
		accountAndPassword = map<string, string>();
		chatRoomList = vector<ChatRoom>();
	}

public:
	static Server& getInstance()
	{
		return *instance;
	}

	map<string, string> getAccountAndPasswordList()
	{
		shared_lock<shared_mutex> lock(accountAndPassword_mutex);

		return accountAndPassword;
	}
	bool addNewAccountAndPassword(string account, string password)
	{
		unique_lock<shared_mutex> lock(accountAndPassword_mutex);

		if (accountAndPassword.find(account) != accountAndPassword.end())
			return false;

		accountAndPassword[account] = password;
	}
	bool checkAccountAndPassword(string account, string password)
	{
		map<string, string> accountAndPassword = getAccountAndPasswordList();

		return accountAndPassword.find(account) != accountAndPassword.end()
			&& accountAndPassword[account] == password;
	}

	vector<ChatRoom> getChatRoomList()
	{

	}
	bool addNewChatRoom(string chatRoomName)
	{
		chatRoomList.emplace_back(7, chatRoomName);
		return true;
	}


	void closeClient(Client* client)
	{
		if (client->status == clientStatus::null_status)
			return;

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
		client->status = clientStatus::null_status;
		closesocket(client->socket);
	}
};
Server* Server::instance = new Server();
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
	if (client->status == clientStatus::null_status)
		return;

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

bool receivePacketIdFromClient(Client* client, clientPacketId* packetId)
{
	char* buff = new char[1] { 0 };

	int hasReceived = recv(client->socket, buff, 1, 0);
	if (hasReceived == SOCKET_ERROR)
	{
		reconnectClientErrorPrint("recv", client);
		return false;
	}

	*packetId = (clientPacketId)to_int(buff, 1);

	return true;
}

bool receiveMsgFromClient(Client* client, ClientPacket* packet)
{
	char* buff = new char[2] { 0 };

	// total length section
	int hasReceived = recv(client->socket, buff, 2, 0);
	if (hasReceived == SOCKET_ERROR)
	{
		reconnectClientErrorPrint("recv", client);
		return false;
	}

	int msgLen = to_int(buff, 2);

	// msg section
	int nowReceive = 0;
	hasReceived = 0;
	char* msgBuff = new char[msgLen] { 0 };
	do
	{
		nowReceive = recv(client->socket, msgBuff + hasReceived, msgLen - hasReceived, 0);
		if (nowReceive == SOCKET_ERROR)
		{
			reconnectClientErrorPrint("recv", client);
			return false;
		}
		hasReceived += nowReceive;
	} while (hasReceived < msgLen);

	packet->setData(msgBuff, msgLen);

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


bool clientLogin(Client* client, bool* loginSuccessOrFailure)
{
	SendLoginDataPacket loginDataPacket;
	if (!receiveMsgFromClient(client, &loginDataPacket))
	{
		reconnectClientErrorPrint("receiveMsgFromClient", client);
		return false;
	}

	*loginSuccessOrFailure = serverInstance.checkAccountAndPassword(loginDataPacket.account, loginDataPacket.password);


	if (!*loginSuccessOrFailure)
	{
		cout << client->ip << " failed to log in" << endl;
	}
	else
	{
		client->setAccountAndPassword(loginDataPacket.account, loginDataPacket.password);
		cout << client->account << "-" << client->ip << " successfully logged in" << endl;
	}

	SuccessOrFailurePacket successOrFailurePacket(*loginSuccessOrFailure);
	if (!sendToClient(client, successOrFailurePacket))
	{
		reconnectClientErrorPrint("sendToClient", client);
		return false;
	}

	return true;
}

bool clientGetChatRoomList(Client* client)
{
	ChatRoomListPacket chatRoomListPacket(serverInstance.chatRoomList);
	if (!sendToClient(client, chatRoomListPacket))
	{
		reconnectClientErrorPrint("sendToClient", client);
		return false;
	}

	return true;
}

bool clientJoinChatRoom(Client* client)
{

	return true;
}


void clientProcess(Client* client)
{
	clientPacketId packetId;

	while (true)
	{
		if (client->status == clientStatus::null_status)
			return;

		if (!receivePacketIdFromClient(client, &packetId))
		{
			reconnectClientErrorPrint("receivePacketIdFromClient", client);
			return;
		}

		switch (packetId)
		{
		case clientPacketId::send_login_data:
			bool loginSuccessOrFailure;
			if (!clientLogin(client, &loginSuccessOrFailure))
				return;
			if (loginSuccessOrFailure)
				client->status = clientStatus::chat_room_list;
			break;

		case clientPacketId::get_chat_room_list:
			if (!clientGetChatRoomList(client))
				return;
			break;

		case clientPacketId::select_chat_room:
			if (!clientJoinChatRoom(client))
				return;
			break;

		case clientPacketId::send_chat_room_message:

				client->status = clientStatus::chat_room;
			break;
		}
	}

}


int main()
{
	//test
	serverInstance.addNewAccountAndPassword("aa", "123");
	serverInstance.addNewAccountAndPassword("2", "123");
	serverInstance.addNewChatRoom("newaa");
	serverInstance.addNewChatRoom("eeeeeeee");
	//test


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

			char client_ip[INET_ADDRSTRLEN] { 0 };
			inet_ntop(AF_INET, &clientAddr, client_ip, INET_ADDRSTRLEN);

			serverInstance.clientList.emplace_back(clientSocket, client_ip);
			serverInstance.clientThreadList.emplace_back(clientProcess, &(serverInstance.clientList.back()));
		}
	}

	return 0;
}
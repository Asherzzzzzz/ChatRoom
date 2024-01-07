#include <iostream>
#include <Winsock2.h>
#include <WS2tcpip.h>
#include <thread>

using namespace std;

#include "ChatRoom.h"
#include "Packet.h"



SOCKET clientSocket;

clientStatus status = clientStatus::initializing;


void exitProgram()
{
	exit(0);
}



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
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		cerr << endl << "socket() failed with error: " << WSAGetLastError() << endl;
		WSACleanup();
		return false;
	}

	return true;
}
#pragma endregion



#pragma region ErrorPrint
void restartErrorPrint(string errorFunctionName)
{
	status = clientStatus::initializing;

	cerr << errorFunctionName << "() failed with error: " << WSAGetLastError() << endl;
	cerr << "Restarting..." << endl;
	WSACleanup();
	closesocket(clientSocket);
	Sleep(RESTART_WAIT_MS);
}

void reconnectErrorPrint(string errorFunctionName)
{
	status = clientStatus::connecting;

	cerr << errorFunctionName << "() failed with error: " << WSAGetLastError() << endl;
	cerr << "Reconnecting..." << endl;
	Sleep(RESTART_WAIT_MS);
}
#pragma endregion



#pragma region Send&Receive
bool sendToServer(SOCKET clientSocket, ClientPacket packet)
{
	int hasSent = 0, nowSend = 0;
	do
	{
		nowSend = send(clientSocket, packet.msgBuff + hasSent, packet.msgLen - hasSent, 0);
		if (nowSend == SOCKET_ERROR)
		{
			return false;
		}
		hasSent += nowSend;
	} while (hasSent != packet.msgLen);

	return true;
}

bool receiveFromServer(SOCKET clientSocket, ServerPacket* packet)
{
	// id section
	int hasReceived = 0;
	char* buff = new char[2];

	hasReceived = recv(clientSocket, buff, 1, 0);
	if (hasReceived == SOCKET_ERROR)
	{
		restartErrorPrint("recv");
		return false;
	}
	
	serverPacketId id = (serverPacketId)to_int(buff, 1);

	// total length section
	hasReceived = recv(clientSocket, buff, 2, 0);
	if (hasReceived == SOCKET_ERROR)
	{
		restartErrorPrint("recv");
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
			restartErrorPrint("recv");
			return false;
		}
		hasReceived += nowReceive;
	} while (hasReceived < msgLen);

	packet->setData(id, msgBuff, msgLen);

	return true;
}
#pragma endregion



#pragma region StatusFunction

bool connectToServer(SOCKET clientSocket)
{
	struct sockaddr_in serverAddr { AF_INET, htons(SERVER_PORT) };
	inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr.s_addr);
	memset(&serverAddr.sin_zero, 0, 8);

	int returnValue = connect(clientSocket, (const sockaddr*)&serverAddr, sizeof(serverAddr));
	if (returnValue == SOCKET_ERROR)
	{
		reconnectErrorPrint("connect");
		return false;
	}

	return true;
}

bool loginToServer(SOCKET clientSocket)
{
	char* account = new char[ACCOUNT_SIZE] { 0 },
		* password = new char[PASSWORD_SIZE] { 0 };

	cout << "Account: ";
	cin.getline(account, ACCOUNT_SIZE);
	cout << "Password: ";
	cin.getline(password, PASSWORD_SIZE);

	SendLoginDataPacket sendLoginDataPacket(account, password);

	if (!sendToServer(clientSocket, sendLoginDataPacket))
	{
		restartErrorPrint("sendToServer");
		return false;
	}

	SuccessOrFailurePacket receiveLoginRespondPacket;

	if (!receiveFromServer(clientSocket, &receiveLoginRespondPacket))
	{
		restartErrorPrint("receiveFromServer");
		return false;
	}
	
	if (!receiveLoginRespondPacket.successOrFailureValue)
	{
		cout << "Failed to login... Press Enter to continue..." << endl;
		cin.get();
		return false;
	}

	return true;
}

bool getChatRoomListFromServer(SOCKET clientSocket, ChatRoom** chatRoomList, int* chatRoomListSize)
{
	GetChatRoomListPacket getChatRoomListPacket;

	if (!sendToServer(clientSocket, getChatRoomListPacket))
	{
		restartErrorPrint("sendToServer");
		return false;
	}

	ChatRoomListPacket receiveChatRoomListPacket;

	if (!receiveFromServer(clientSocket, &receiveChatRoomListPacket))
	{
		restartErrorPrint("receiveFromServer");
		return false;
	}

	*chatRoomListSize = receiveChatRoomListPacket.chatRoomListSize;
	*chatRoomList = receiveChatRoomListPacket.chatRoomList;
}

bool selectChatRoom(SOCKET clientSocket)
{
	ChatRoom* chatRoomList;
	int chatRoomListSize = 0;
	if (getChatRoomListFromServer(clientSocket, &chatRoomList, &chatRoomListSize))
	{
		for (int i = 0; i < chatRoomListSize; i++)
		{
			printf("%d %s\n", chatRoomList[i].id, chatRoomList[i].name.c_str());
		}
	}

	return true;
}
#pragma endregion



void test()
{
	cout << "--test start-- " << endl;
	cin.getline(new char[1], 1);
	cout << "--test end--" << endl;
}


int main()
{
	//test();

	WSADATA wsaData;

	while (1)
	{
		switch (status)
		{
		case clientStatus::initializing:
			if (initializeWSA(&wsaData) && initializeSocket())
				status = clientStatus::connecting;
			break;

		case clientStatus::connecting:
			if (connectToServer(clientSocket))
				status = clientStatus::loggingIn;
			break;

		case clientStatus::loggingIn:
			if (loginToServer(clientSocket))
				status = clientStatus::chatRoomList;
			break;

		case clientStatus::chatRoomList:
			if (selectChatRoom(clientSocket))
				status = clientStatus::chatRoom;
			break;

		case clientStatus::chatRoom:

			break;
		}
	}

	closesocket(clientSocket);
	WSACleanup();
}
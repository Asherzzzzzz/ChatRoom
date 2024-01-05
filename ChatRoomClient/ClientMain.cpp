#include <iostream>
#include <Winsock2.h>
#include <WS2tcpip.h>
#include <thread>

using namespace std;



#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9909
#define RECONNECT_WAIT_MS 5000

#define BUFF_SIZE 1024
#define ACCOUNT_SIZE 50
#define PASSWORD_SIZE 50
#define LOGIN_PACKET_SIZE 6 + ACCOUNT_SIZE + PASSWORD_SIZE



enum sendPacketId
{
	checkStatus,
	sendLoginData,
	getChatRoomList,
	sendChatRoomMessage
};
enum receivePacketId
{
	returnStatus,
	successOrFailure,
	ChatRoomList,
	ChatRoomMessage
};
enum status
{
	connecting,
	login,
	chatRoomList,
	chatRoom
};



#pragma region TypeConversion
char to_char(int n)
{
	return '0' + n;
}

int to_int(const char* c, int cLen)
{
	int int_c = 0;

	for (int i = 0; i < cLen; i++)
	{
		int_c *= 10;
		int_c += c[i] - '0';
	}

	return int_c;
}
#pragma endregion



#pragma region Initialization
void initializeWSA(WSADATA* wsaData)
{
	int error = WSAStartup(MAKEWORD(2, 2), wsaData);
	if (error != 0)
	{
		cerr << "WSAStartup() failed with error: " << error << endl;
		exit(EXIT_FAILURE);
	}
}

void initializeSocket(SOCKET* clientSocket)
{
	*clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (*clientSocket == INVALID_SOCKET)
	{
		cerr << endl << "socket() failed with error: " << WSAGetLastError() << endl;
		WSACleanup();
		exit(EXIT_FAILURE);
	}
}
#pragma endregion



#pragma region Send&Receive
void reconnectErrorPrint(string errorFunctionName)
{
	cerr << errorFunctionName << "() failed with error: " << WSAGetLastError() << endl;
	cerr << "Reconnecting..." << endl;
	Sleep(RECONNECT_WAIT_MS);
}

bool sentToServer(SOCKET clientSocket, const char* buff, int buffLen)
{
	int hasSent = 0, nowSend = 0;
	do
	{
		nowSend = send(clientSocket, buff + hasSent, buffLen - hasSent, 0);
		if (nowSend == SOCKET_ERROR)
		{
			return false;
		}
		hasSent += nowSend;
	} while (hasSent != buffLen);

	return true;
}

bool receiveFromServer(SOCKET clientSocket, char** msgBuff, int* msgBuffLen)
{
	int hasReceived = 0;
	char* buff = new char[2];

	hasReceived = recv(clientSocket, buff, 1, 0);
	if (hasReceived == SOCKET_ERROR)
	{
		reconnectErrorPrint("recv");
		return false;
	}
	
	receivePacketId id = (receivePacketId)to_int(buff, 1);

	hasReceived = recv(clientSocket, buff, 2, 0);
	if (hasReceived == SOCKET_ERROR)
	{
		reconnectErrorPrint("recv");
		return false;
	}

	*msgBuffLen = to_int(buff, 2);
	
	int nowReceive = 0;
	hasReceived = 0;
	*msgBuff = new char[*msgBuffLen] { 0 };
	do
	{
		nowReceive = recv(clientSocket, *msgBuff + hasReceived, *msgBuffLen - hasReceived, 0);
		if (nowReceive == SOCKET_ERROR)
		{
			reconnectErrorPrint("recv");
			return false;
		}
		hasReceived += nowReceive;
	} while (hasReceived < *msgBuffLen);

	return true;
}
#pragma endregion



#pragma region DataToPacket
int toLoginPacket(const char* account, const char* password, char* outputPacket)
{
	//packet_id + accountSize + account + passwordSize + password + \0
	if (outputPacket)
	{
		outputPacket[0] = to_char(sendPacketId::sendLoginData);
		outputPacket[1] = to_char(strlen(account) / 10);
		outputPacket[2] = to_char(strlen(account) % 10);
		strcat_s(outputPacket, 106, account);
		outputPacket[strlen(account) + 3] = to_char(strlen(password) / 10);
		outputPacket[strlen(account) + 4] = to_char(strlen(password) % 10);
		outputPacket[strlen(account) + 5] = '\0';
		strcat_s(outputPacket, 106, password);

		return strlen(outputPacket);
	}

	return -1;
}
#pragma endregion

void exitProgram()
{
	exit(0);
}


#pragma region StatusFunction

bool connectToServer(SOCKET clientSocket, sockaddr_in serverAddr)
{
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
	char* packetBuff = new char[LOGIN_PACKET_SIZE] { 0 }, //packet_id + accountSize + account + passwordSize + password + endingChar
		* account = new char[ACCOUNT_SIZE] { 0 },
		* password = new char[PASSWORD_SIZE] { 0 };

	cout << "Account: ";
	cin.getline(account, ACCOUNT_SIZE);
	cout << "Password: ";
	cin.getline(password, PASSWORD_SIZE);

	int packetLen = toLoginPacket(account, password, packetBuff);

	if (!sentToServer(clientSocket, packetBuff, packetLen))
	{
		reconnectErrorPrint("sendToServer");
		return false;
	}

	char* msgBuff = new char[BUFF_SIZE] { 0 };
	int* msgBuffLen = new int(BUFF_SIZE);

	if (!receiveFromServer(clientSocket, &msgBuff, msgBuffLen))
	{
		reconnectErrorPrint("sendToServer");
		return false;
	}
	
	if (to_int(msgBuff, *msgBuffLen) == 0)
	{
		cout << "Failed to login... Press Enter to continue..." << endl;
		cin.get();
		return false;
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
	SOCKET clientSocket;
	initializeWSA(&wsaData);
	initializeSocket(&clientSocket);


	struct sockaddr_in serverAddr { AF_INET, htons(SERVER_PORT) };
	inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr.s_addr);
	memset(&serverAddr.sin_zero, 0, 8);


	status clientStatus = status::connecting;

	while (1)
	{
		switch (clientStatus)
		{
		case status::connecting:
			if (connectToServer(clientSocket, serverAddr))
				clientStatus = status::login;

			break;

		case status::login:
			if (loginToServer(clientSocket))
				clientStatus = status::chatRoomList;

			break;

		case status::chatRoomList:
			
			break;

		case status::chatRoom:

			break;
		}
	}

	closesocket(clientSocket);
	WSACleanup();
}
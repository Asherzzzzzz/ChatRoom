#include <iostream>
#include <Winsock2.h>
#include <WS2tcpip.h>
#include <thread>

using namespace std;

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9909
#define RECONNECT_WAIT_MS 5000

#define ACCOUNT_MAX_SIZE 50
#define PASSWORD_MAX_SIZE 50
#define LOGIN_PACKET_MAX_SIZE 6 + ACCOUNT_MAX_SIZE + PASSWORD_MAX_SIZE


enum packetId
{
	checkStatus,
	login,
	getChatRoomList,
	sendChatRoomMessage
};

enum status
{
	success,
	error,
	notLogin,
	chatRoomList,
	inChatRoom
};


char to_char(int n)
{
	return '0' + n;
}

bool sentToServer(SOCKET s, const char* buff, int buffLen)
{
	int hasSent = 0;
	do
	{
		hasSent = send(s, buff + hasSent, buffLen - hasSent, 0);
		if (hasSent == SOCKET_ERROR)
		{
			cerr << "send() failed with error: " << WSAGetLastError() << endl;
			return false;
		}
	} while (hasSent != buffLen);

	return true;
}

int toLoginPacket(char* account, char* password, char* outputPacket)
{
	//packet_id + account + accountSize + password + passwordSize + \0
	if (outputPacket)
	{
		outputPacket[0] = to_char(login);
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


void test()
{
	
}


int main()
{
	test();

	WSADATA wsaData;
	int error = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (error != 0)
	{
		cerr << "WSAStartup() failed with error: " << error << endl;
		exit(EXIT_FAILURE);
	}

	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		cerr << endl << "socket() failed with error: " << WSAGetLastError() << endl;
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in serverAddr { AF_INET, htons(SERVER_PORT) };
	inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr.s_addr);
	memset(&serverAddr.sin_zero, 0, 8);

	while (1)
	{
		system("cls");

		int returnValue = connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
		if (returnValue == SOCKET_ERROR)
		{
			cerr << "connect() failed with error: " << WSAGetLastError() << endl;
			cerr << "Reconnecting..." << endl;
			Sleep(RECONNECT_WAIT_MS);
			continue;
		}

		
		char* packetBuff = new char[LOGIN_PACKET_MAX_SIZE] { 0 }, //packet_id + account + accountSize + password + passwordSize + endingChar
			* account = new char[ACCOUNT_MAX_SIZE] { 0 }, 
			* password = new char[PASSWORD_MAX_SIZE] { 0 };
		
		cout << "Account: ";
		cin.getline(account, ACCOUNT_MAX_SIZE);
		cout << "Password: ";
		cin.getline(password, PASSWORD_MAX_SIZE);

		int packetLen = toLoginPacket(account, password, packetBuff);
		
		if (!sentToServer(clientSocket, packetBuff, packetLen))
		{
			cerr << "Reconnecting..." << endl;
			Sleep(RECONNECT_WAIT_MS);
			continue;
		}
		
	}
	


	//system("cls");
}
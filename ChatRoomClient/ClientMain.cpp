#include <iostream>
#include <Winsock2.h>
#include <WS2tcpip.h>
#include <thread>

using namespace std;

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9909
#define RECONNECT_WAIT_MS 5000

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

		
		char packetBuff[106] = { 0 }; //packet_id + account + accountSize + password + passwordSize + endingChar
		char account[50], password[50];
		
		cout << "Account: ";
		cin.getline(account, 50);
		cout << "Password: ";
		cin.getline(password, 50);

		packetBuff[0] = to_char(login);
		packetBuff[1] = to_char(strlen(account) / 10);
		packetBuff[2] = to_char(strlen(account) % 10);
		strcat_s(packetBuff, 106, account);
		packetBuff[strlen(account) + 3] = to_char(strlen(password) / 10);
		packetBuff[strlen(account) + 4] = to_char(strlen(password) % 10);
		packetBuff[strlen(account) + 5] = '\0';
		strcat_s(packetBuff, 106, password);

		returnValue = send(clientSocket, packetBuff, strlen(packetBuff), 0);
		if (returnValue == SOCKET_ERROR)
		{
			cerr << "send() failed with error: " << WSAGetLastError() << endl;
			
		}
	}
	


	//system("cls");
}
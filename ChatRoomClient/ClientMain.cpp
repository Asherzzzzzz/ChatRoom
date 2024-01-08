#include <iostream>
#include <Winsock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <conio.h>
#include <mutex>

using namespace std;

#include "ChatRoom.h"
#include "Packet.h"



SOCKET clientSocket;
clientStatus status = clientStatus::initializing;
string clientAccount;
vector<ChatRoomMsg> chatRoomMsgList = vector<ChatRoomMsg>();


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
bool sendToServer(ClientPacket packet)
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

bool receiveFromServer(ServerPacket* packet)
{
	// id section
	int hasReceived = 0;
	char* buff = new char[packet->getTotalSizeLength()];

	hasReceived = recv(clientSocket, buff, 1, 0);
	if (hasReceived == SOCKET_ERROR)
	{
		restartErrorPrint("recv");
		return false;
	}
	
	serverPacketId id = (serverPacketId)to_int(buff, 1);

	// total length section
	hasReceived = recv(clientSocket, buff, packet->getTotalSizeLength(), 0);
	if (hasReceived == SOCKET_ERROR)
	{
		restartErrorPrint("recv");
		return false;
	}

	int msgLen = to_int(buff, packet->getTotalSizeLength());

	// msg section
	int nowReceive = 0;
	hasReceived = 0;
	char* msgBuff = new char[BUFF_SIZE] { 0 };
	while (hasReceived < msgLen)
	{
		nowReceive = recv(clientSocket, msgBuff + hasReceived, msgLen - hasReceived, 0);
		if (nowReceive == SOCKET_ERROR)
		{
			restartErrorPrint("recv");
			return false;
		}
		hasReceived += nowReceive;
	} 

	packet->setData(msgBuff, msgLen);

	return true;
}
#pragma endregion



#pragma region Connect&Login
bool connectToServer()
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

bool signUpOrLogin()
{
	system("cls");
	cout << "Command: /signup /login" << endl;

	string cmd;
	cin >> cmd;
	cin.get();

	if (cmd == "/signup")
	{
		cout << "Account and password length must less than " << ACCOUNT_SIZE << " characters" << endl;

		char* account = new char[ACCOUNT_SIZE] { 0 },
			* password = new char[PASSWORD_SIZE] { 0 };

		cout << "Account: ";
		cin.getline(account, ACCOUNT_SIZE);
		cout << "Password: ";
		cin.getline(password, PASSWORD_SIZE);

		if (strlen(account) == 0 || strlen(password) == 0)
		{
			cout << "Account and password can't be empty... Press Enter to continue..." << endl;
			cin.get();
			return false;
		}

		SendSignUpDataPacket sendSignUpDataPacket(account, password);

		if (!sendToServer(sendSignUpDataPacket))
		{
			restartErrorPrint("sendToServer");
			return false;
		}

		SuccessOrFailurePacket receiveSignUpRespondPacket;

		if (!receiveFromServer(&receiveSignUpRespondPacket))
		{
			restartErrorPrint("receiveFromServer");
			return false;
		}

		if (!receiveSignUpRespondPacket.successOrFailureValue)
		{
			cout << "Failed to sign up... Press Enter to continue..." << endl;
			cin.get();
			return false;
		}

		cout << "Successfully signed up, Press Enter to continue..." << endl;
		cin.get();
		return false;
	}
	else if (cmd == "/login")
	{
		cout << "Account and password length must less than " << ACCOUNT_SIZE << " characters" << endl;

		char* account = new char[ACCOUNT_SIZE] { 0 },
			* password = new char[PASSWORD_SIZE] { 0 };

		cout << "Account: ";
		cin.getline(account, ACCOUNT_SIZE);
		cout << "Password: ";
		cin.getline(password, PASSWORD_SIZE);

		SendLoginDataPacket sendLoginDataPacket(account, password);

		if (!sendToServer(sendLoginDataPacket))
		{
			restartErrorPrint("sendToServer");
			return false;
		}

		SuccessOrFailurePacket receiveLoginRespondPacket;

		if (!receiveFromServer(&receiveLoginRespondPacket))
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

		clientAccount = account;
		return true;
	}
	else
	{
		cout << cmd << " <-- unknown command" << endl;
		cout << "Press enter to continue..." << endl;
		cin.get();
		return false;
	}

	return true;
}
#pragma endregion


#pragma region SelectChatRoomProcess
mutex joinChatRoom_mutex;
void showChatRoomList(bool* hasSelectedChatRoom, string* input, bool* noErrorOccur)
{
	while (!*hasSelectedChatRoom)
	{
		{
			lock_guard<mutex> lock(joinChatRoom_mutex);

			GetChatRoomListPacket getChatRoomListPacket;

			if (!sendToServer(getChatRoomListPacket))
			{
				restartErrorPrint("sendToServer");
				*noErrorOccur = false;
				return;
			}

			ChatRoomListPacket receiveChatRoomListPacket;

			if (!receiveFromServer(&receiveChatRoomListPacket))
			{
				restartErrorPrint("receiveFromServer");
				*noErrorOccur = false;
				return;
			}

			system("cls");

			if (receiveChatRoomListPacket.chatRoomList.empty())
			{
				cout << "There's no existing chat room now..." << endl;
			}

			for (ChatRoom chatRoom : receiveChatRoomListPacket.chatRoomList)
			{
				printf_s("[%d] %s\n", chatRoom.id, chatRoom.name.c_str());
			}

			cout << *input;
		}
		
		this_thread::sleep_for(chrono::milliseconds(GET_CHAT_ROOM_LIST_WAIT_MS));
	}
}
void getClientChatRoomNameInput(string* input)
{
	const int BACKSPACE = 8;
	//const int DEL = 127; // delete

	char c;

	while (true)
	{
		if (_kbhit())
		{
			lock_guard<mutex> lock(joinChatRoom_mutex);

			c = _getch();

			if (c >= ' ' && c <= '~')
			{
				input->push_back(c);
				cout << c;
			}
			else if (c == BACKSPACE)
			{
				if (!input->empty())
				{
					input->pop_back();
					cout << '\b';
				}

				cout << " \b";
			}
			else if (c == '\r')
			{
				return;
			}
		}
	}
}
bool joinChatRoom()
{
	bool hasSelectedChatRoom = false, noErrorOccur = true;
	string input = "";

	thread showList(showChatRoomList, &hasSelectedChatRoom, &input, &noErrorOccur);
	thread getInput(getClientChatRoomNameInput, &input);
	
	getInput.join();
	hasSelectedChatRoom = true;
	showList.detach();

	if (!noErrorOccur)
		return false;

	SelectChatRoomPacket selectChatRoomPacket(input);

	if (!sendToServer(selectChatRoomPacket))
	{
		restartErrorPrint("sendToServer");
		return false;
	}

	SuccessOrFailurePacket receiveJoinRespondPacket;

	if (!receiveFromServer(&receiveJoinRespondPacket))
	{
		restartErrorPrint("receiveFromServer");
		return false;
	}

	return receiveJoinRespondPacket.successOrFailureValue;
}
#pragma endregion


#pragma region InChatRoomProcess
mutex inChatRoom_mutex;
void showChatRoomMsg(bool* hasQuit, string* input, bool* noErrorOccur)
{
	while (!*hasQuit)
	{
		{
			lock_guard<mutex> lock(mutex inChatRoom_mutex);

			GetChatRoomMsgPacket getChatRoomMsgPacket(chatRoomMsgList.size());

			if (!sendToServer(getChatRoomMsgPacket))
			{
				restartErrorPrint("sendToServer");
				*noErrorOccur = false;
				return;
			}

			ChatRoomMsgPacket receiveChatRoomMsgPacket;

			if (!receiveFromServer(&receiveChatRoomMsgPacket))
			{
				restartErrorPrint("receiveFromServer");
				*noErrorOccur = false;
				return;
			}

			system("cls");

			for (int i = 0; i < receiveChatRoomMsgPacket.chatRoomMsgList.size(); i++)
			{
				// change format (self msg)
				printf_s("<%s> - %s\n\n", receiveChatRoomMsgPacket.chatRoomMsgList[i].sender.c_str(), receiveChatRoomMsgPacket.chatRoomMsgList[i].msg.c_str());
			}

			cout << *input;
		}
		
		this_thread::sleep_for(chrono::milliseconds(GET_CHAT_ROOM_MSG_WAIT_MS));
	}
}
void sendChatRoomMsgToServer(string input)
{
	SendChatRoomMsgPacket sendChatRoomMsgPacket(ChatRoomMsg(clientAccount, input));

	if (!sendToServer(sendChatRoomMsgPacket))
	{
		restartErrorPrint("sendToServer");
		return;
	}

	SuccessOrFailurePacket receiveSendRespondPacket;

	if (!receiveFromServer(&receiveSendRespondPacket))
	{
		restartErrorPrint("receiveFromServer");
		return;
	}

	if (!receiveSendRespondPacket.successOrFailureValue)
	{
		cout << input << " <-- Failed to send to server..." << endl;
		return;
	}
}
void getClientChatRoomMsgInput(string* input)
{
	const int BACKSPACE = 8;
	//const int DEL = 127; // delete

	char c;

	while (true)
	{
		if (_kbhit())
		{
			lock_guard<mutex> lock(mutex inChatRoom_mutex);

			c = _getch();

			if (c == '\'' || c == '"')
			{
				continue;
			}
			else if (c >= ' ' && c <= '~')
			{
				input->push_back(c);
				cout << c;
			}
			else if (c == BACKSPACE)
			{
				if (!input->empty())
				{
					input->pop_back();
					cout << '\b';
				}
					
				cout << " \b";
			}
			else if (c == '\r')
			{
				sendChatRoomMsgToServer(*input);
				return;
			}
		}
	}
}
bool inChatRoom()
{
	bool hasQuit = false, noErrorOccur = true;
	string input = "";

	thread showMsg(showChatRoomMsg, &hasQuit, &input, &noErrorOccur);
	thread getInput(getClientChatRoomMsgInput, &input);

	getInput.join();
	hasQuit = true;
	showMsg.join();

	if (!noErrorOccur)
		return false;

	return true;
}
#pragma endregion



void test()
{
	cout << "--test start-- " << endl;

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
			if (connectToServer())
				status = clientStatus::logging_in;
			break;

		case clientStatus::logging_in:
			if (signUpOrLogin())
				status = clientStatus::chat_room_list;
			break;

		case clientStatus::chat_room_list:
			if (joinChatRoom())
				status = clientStatus::chat_room;
			break;

		case clientStatus::chat_room:
			if (!inChatRoom())
				status = clientStatus::chat_room_list;
			break;
		}
	}

	closesocket(clientSocket);
	WSACleanup();
}
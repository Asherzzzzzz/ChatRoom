#ifndef PACKET_HEADER
#define PACKET_HEADER

#include <string>
#include <cstring>
#include <vector>

#include "ChatRoom.h"

using namespace std;


#pragma region ClientPacket
class ClientPacket
{
public:
	clientPacketId id;
	char* msgBuff;
	int msgLen;

	ClientPacket();
	virtual int getTotalSizeLength();
	virtual void setData(char*, int);
};

class SendLoginDataPacket : public ClientPacket
{
public:
	string account, password;

	SendLoginDataPacket();
	SendLoginDataPacket(string, string);
	void setData(char*, int);
};

class GetChatRoomListPacket : public ClientPacket
{
public:
	GetChatRoomListPacket();
	//void setData(char*, int);
};
#pragma endregion


#pragma region ServerPacket
class ServerPacket
{
public:
	serverPacketId id;
	char* msgBuff;
	int msgLen;

	ServerPacket();
	virtual int getTotalSizeLength();
	virtual void setData(char*, int);
};

class SuccessOrFailurePacket : public ServerPacket
{
public:
	bool successOrFailureValue;

	SuccessOrFailurePacket();
	SuccessOrFailurePacket(bool);
	void setData(char*, int);
};

class ChatRoomListPacket : public ServerPacket
{
public:
	vector<ChatRoom> chatRoomList;

	ChatRoomListPacket();
	ChatRoomListPacket(vector<ChatRoom>);
	int getTotalSizeLength();
	void setData(char*, int);
};
#pragma endregion

#endif
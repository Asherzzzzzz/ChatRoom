#ifndef PACKET_HEADER
#define PACKET_HEADER

#include <string>
#include <string.h>
#include "ChatRoom.h"


#pragma region ClientPacket
class ClientPacket
{
public:
	clientPacketId id;
	char* msgBuff;
	int msgLen;

	ClientPacket();
	virtual void setData(clientPacketId, char*, int);
};

class SendLoginDataPacket : public ClientPacket
{
public:
	std::string account, password;

	SendLoginDataPacket();
	SendLoginDataPacket(const char*, const char*);
	virtual void setData(clientPacketId, char*, int);
};

class GetChatRoomListPacket : public ClientPacket
{
public:
	GetChatRoomListPacket();
	virtual void setData(clientPacketId, char*, int);
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
	virtual void setData(serverPacketId, char*, int);
};

class SuccessOrFailurePacket : public ServerPacket
{
public:
	bool successOrFailureValue;

	SuccessOrFailurePacket();
	virtual void setData(serverPacketId, char*, int);
};

class ChatRoomListPacket : public ServerPacket
{
public:
	ChatRoom* chatRoomList;
	int chatRoomListSize;

	ChatRoomListPacket();
	virtual void setData(serverPacketId, char*, int);
};
#pragma endregion

#endif
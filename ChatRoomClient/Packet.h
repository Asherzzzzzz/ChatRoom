#pragma once

#define BUFF_SIZE 1024

#define ACCOUNT_SIZE 50
#define PASSWORD_SIZE 50
#define LOGIN_PACKET_SIZE 6 + ACCOUNT_SIZE + PASSWORD_SIZE

#define GET_CHAT_ROOM_LIST_PACKET_SIZE 2


enum sendPacketId
{
	checkStatus,
	sendLoginData,
	getChatRoomList,
	sendChatRoomMessage
};
enum receivePacketId
{
	successOrFailure,
	returnStatus,
	ChatRoomList,
	ChatRoomMessage
};


#pragma region SendPacket
class SendPacket
{
public:
	char* msgBuff;
	int msgLen;

protected:
	SendPacket();
};

class SendLoginDataPacket : public SendPacket
{
public:
	SendLoginDataPacket(const char*, const char*);
};

class GetChatRoomListPacket : public SendPacket
{
public:
	GetChatRoomListPacket();
};
#pragma endregion


#pragma region ReceivePacket
class ReceivePacket
{
public:
	receivePacketId id;
	char* msgBuff;
	int msgLen;

	ReceivePacket();
};
#pragma endregion
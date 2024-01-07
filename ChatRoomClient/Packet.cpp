#include "Packet.h"


#pragma region ClientPacket
ClientPacket::ClientPacket()
{
	id = clientPacketId::sendLoginData;
	msgBuff = new char[0];
	msgLen = 0;
}
void ClientPacket::setData(clientPacketId id, char* msgBuff, int msgLen)
{
	this->id = id;
	this->msgBuff = msgBuff;
	this->msgLen = msgLen;
}


SendLoginDataPacket::SendLoginDataPacket()
{
	account = "";
	password = "";
}
SendLoginDataPacket::SendLoginDataPacket(const char* account, const char* password)
{
	int buffIndex = 0;
	msgBuff = new char[BUFF_SIZE] { 0 };

	//packet_id + totoalSize + accountSize + account + passwordSize + password + \0
	msgBuff[buffIndex++] = to_char(clientPacketId::sendLoginData);

	msgBuff[buffIndex++] = to_char((strlen(account) + strlen(password) + 4) / 10);
	msgBuff[buffIndex++] = to_char((strlen(account) + strlen(password) + 4) % 10);

	msgBuff[buffIndex++] = to_char(strlen(account) / 10);
	msgBuff[buffIndex++] = to_char(strlen(account) % 10);

	strcat_s(msgBuff, 106, account);
	buffIndex += strlen(account);

	msgBuff[buffIndex++] = to_char(strlen(password) / 10);
	msgBuff[buffIndex++] = to_char(strlen(password) % 10);
	msgBuff[buffIndex++] = '\0';

	strcat_s(msgBuff, 106, password);
	buffIndex += strlen(password);

	msgBuff[buffIndex++] = '\0';

	msgLen = strlen(msgBuff);
}
void SendLoginDataPacket::setData(clientPacketId id, char* msgBuff, int msgLen)
{
	ClientPacket::setData(id, msgBuff, msgLen);

	int hasRead = 0;

	int accountLen = to_int(msgBuff + hasRead, 2);
	hasRead += 2;
	account.assign(msgBuff + hasRead, msgBuff + hasRead + accountLen);
	hasRead += accountLen;

	int passwordLen = to_int(msgBuff + hasRead, 2);
	hasRead += 2;
	password.assign(msgBuff + hasRead, msgBuff + hasRead + passwordLen);
	hasRead += passwordLen;
}


GetChatRoomListPacket::GetChatRoomListPacket()
{
	int buffIndex = 0;
	msgBuff = new char[BUFF_SIZE] { 0 };

	msgBuff[buffIndex++] = to_char(clientPacketId::getChatRoomList);
	msgBuff[buffIndex++] = '\0';

	msgLen = strlen(msgBuff);
}
void GetChatRoomListPacket::setData(clientPacketId id, char* msgBuff, int msgLen)
{
	ClientPacket::setData(id, msgBuff, msgLen);
}
#pragma endregion



#pragma region ServerPacket
ServerPacket::ServerPacket()
{
	id = serverPacketId::receiveSuccessOrFailure;
	msgBuff = new char[BUFF_SIZE] { 0 };
	msgLen = BUFF_SIZE;
}
void ServerPacket::setData(serverPacketId id, char* msgBuff, int msgLen)
{
	this->id = id;
	this->msgBuff = msgBuff;
	this->msgLen = msgLen;
}


SuccessOrFailurePacket::SuccessOrFailurePacket()
{
	id = serverPacketId::receiveSuccessOrFailure;
	successOrFailureValue = false;
}
void SuccessOrFailurePacket::setData(serverPacketId id, char* msgBuff, int msgLen)
{
	ServerPacket::setData(id, msgBuff, msgLen);

	successOrFailureValue = to_int(msgBuff, msgLen);
}


ChatRoomListPacket::ChatRoomListPacket()
{
	id = serverPacketId::receiveChatRoomList;
	chatRoomList = new ChatRoom[0];
	chatRoomListSize = 0;
}
void ChatRoomListPacket::setData(serverPacketId id, char* msgBuff, int msgLen)
{
	ServerPacket::setData(id, msgBuff, msgLen);

	int hasRead = 0;

	chatRoomListSize = to_int(msgBuff + hasRead, 2);
	hasRead += 2;
	chatRoomList = new ChatRoom[chatRoomListSize];

	int chatRoomId, chatRoomNameLen;
	std::string chatRoomName;
	for (int i = 0; i < chatRoomListSize; i++)
	{
		if (hasRead >= msgLen)
			break;

		chatRoomId = to_int(msgBuff + hasRead, 2);
		hasRead += 2;
		chatRoomNameLen = to_int(msgBuff + hasRead, 2);
		hasRead += 2;
		chatRoomName.assign(msgBuff + hasRead, msgBuff + hasRead + chatRoomNameLen);
		hasRead += chatRoomNameLen;
	}
}
#pragma endregion
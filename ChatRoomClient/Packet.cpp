#include "Packet.h"


#pragma region ClientPacket
ClientPacket::ClientPacket()
{
	id = clientPacketId::send_login_data;
	msgBuff = new char[0];
	msgLen = 0;
}
int ClientPacket::getTotalSizeLength()
{
	return NORMAL_TOTAL_SIZE_LENGTH;
}
void ClientPacket::setData(char* msgBuff, int msgLen)
{
	this->msgBuff = msgBuff;
	this->msgLen = msgLen;
}


SendLoginDataPacket::SendLoginDataPacket()
{
	id = clientPacketId::send_login_data;
	account = "";
	password = "";
}
SendLoginDataPacket::SendLoginDataPacket(string account, string password)
{
	id = clientPacketId::send_login_data;
	this->account = account;
	this->password = password;

	int buffIndex = 0;
	msgBuff = new char[BUFF_SIZE] { 0 };

	//packetId(1) + totoalSize(2) + accountSize(2) + account + passwordSize(2) + password + \0
	msgBuff[buffIndex++] = to_char(clientPacketId::send_login_data);

	msgBuff[buffIndex++] = to_char((account.length() + password.length() + 4) / 10);
	msgBuff[buffIndex++] = to_char((account.length() + password.length() + 4) % 10);

	msgBuff[buffIndex++] = to_char(account.length() / 10);
	msgBuff[buffIndex++] = to_char(account.length() % 10);
	msgBuff[buffIndex] = '\0';

	strcat_s(msgBuff, buffIndex + account.length() + 1, account.c_str());
	buffIndex += account.length();

	msgBuff[buffIndex++] = to_char(password.length() / 10);
	msgBuff[buffIndex++] = to_char(password.length() % 10);
	msgBuff[buffIndex] = '\0';

	strcat_s(msgBuff, buffIndex + password.length() + 1, password.c_str());
	buffIndex += password.length();

	msgLen = strlen(msgBuff);
}
void SendLoginDataPacket::setData(char* msgBuff, int msgLen)
{
	ClientPacket::setData(msgBuff, msgLen);

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
	id = clientPacketId::get_chat_room_list;

	int buffIndex = 0;
	msgBuff = new char[BUFF_SIZE] { 0 };

	msgBuff[buffIndex++] = to_char(clientPacketId::get_chat_room_list);
	msgBuff[buffIndex] = '\0';

	msgLen = strlen(msgBuff);
}
#pragma endregion



#pragma region ServerPacket
ServerPacket::ServerPacket()
{
	id = serverPacketId::receive_success_or_failure;
	msgBuff = new char[BUFF_SIZE] { 0 };
	msgLen = BUFF_SIZE;
}
int ServerPacket::getTotalSizeLength()
{
	return NORMAL_TOTAL_SIZE_LENGTH;
}
void ServerPacket::setData(char* msgBuff, int msgLen)
{
	this->msgBuff = msgBuff;
	this->msgLen = msgLen;
}


SuccessOrFailurePacket::SuccessOrFailurePacket()
{
	id = serverPacketId::receive_success_or_failure;
	successOrFailureValue = false;
}
SuccessOrFailurePacket::SuccessOrFailurePacket(bool successOrFailureValue)
{
	id = serverPacketId::receive_success_or_failure;
	this->successOrFailureValue = successOrFailureValue;

	int buffIndex = 0;
	msgBuff = new char[BUFF_SIZE] { 0 };

	//packetId(1) + totoalSize(2) + successOrFailure(1) + \0
	msgBuff[buffIndex++] = to_char(id);

	msgBuff[buffIndex++] = '0';
	msgBuff[buffIndex++] = '1';

	msgBuff[buffIndex++] = to_char((int)successOrFailureValue);
	msgBuff[buffIndex] = '\0';

	msgLen = strlen(msgBuff);
}
void SuccessOrFailurePacket::setData(char* msgBuff, int msgLen)
{
	ServerPacket::setData(msgBuff, msgLen);
	successOrFailureValue = to_int(msgBuff, 1);
}


ChatRoomListPacket::ChatRoomListPacket()
{
	id = serverPacketId::receive_chat_room_list;
	chatRoomList = vector<ChatRoom>();
}
ChatRoomListPacket::ChatRoomListPacket(vector<ChatRoom> chatRoomList)
{
	id = serverPacketId::receive_chat_room_list;
	this->chatRoomList = chatRoomList;

	int buffIndex = 0;
	msgBuff = new char[BUFF_SIZE] { 0 };

	//packetId(1) + totoalSize(4) + roomId(2) + roomNameSize(2) + roomName + ... + \0
	msgBuff[buffIndex++] = to_char(id);

	int totalSizeIndex = buffIndex;
	buffIndex += 4;
	int totalSize = 0;

	for (ChatRoom chatRoom : chatRoomList)
	{
		msgBuff[buffIndex++] = to_char(chatRoom.id / 10);
		msgBuff[buffIndex++] = to_char(chatRoom.id % 10);

		msgBuff[buffIndex++] = to_char(chatRoom.name.length() / 10);
		msgBuff[buffIndex++] = to_char(chatRoom.name.length() % 10);
		msgBuff[buffIndex] = '\0';

		strcat_s(msgBuff + buffIndex, buffIndex + chatRoom.name.length() + 1, chatRoom.name.c_str());
		buffIndex += chatRoom.name.length();

		totalSize += 4 + chatRoom.name.length();
	}

	msgBuff[totalSizeIndex++] = to_char(totalSize / 1000);
	msgBuff[totalSizeIndex++] = to_char(totalSize / 100 % 10);
	msgBuff[totalSizeIndex++] = to_char(totalSize / 10 % 10);
	msgBuff[totalSizeIndex++] = to_char(totalSize % 10);

	msgLen = strlen(msgBuff);
}
int ChatRoomListPacket::getTotalSizeLength()
{
	return GET_CHAT_ROOM_LIST_TOTAL_SIZE_LENGTH;
}
void ChatRoomListPacket::setData(char* msgBuff, int msgLen)
{
	ServerPacket::setData(msgBuff, msgLen);

	int hasRead = 0;

	int chatRoomId, chatRoomNameLen;
	string chatRoomName;
	while (hasRead < msgLen)
	{
		chatRoomId = to_int(msgBuff + hasRead, 2);
		hasRead += 2;
		chatRoomNameLen = to_int(msgBuff + hasRead, 2);
		hasRead += 2;
		chatRoomName.assign(msgBuff + hasRead, msgBuff + hasRead + chatRoomNameLen);
		hasRead += chatRoomNameLen;

		chatRoomList.emplace_back(chatRoomId, chatRoomName);
	}
}
#pragma endregion
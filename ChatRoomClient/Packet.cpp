#include "Packet.h"

#include <string.h>
#include "TypeConversion.h"


#pragma region SendPacket
SendPacket::SendPacket()
{
	msgBuff = new char[0];
	msgLen = 0;
}


SendLoginDataPacket::SendLoginDataPacket(const char* account, const char* password)
{
	msgBuff = new char[LOGIN_PACKET_SIZE] { 0 };

	//packet_id + accountSize + account + passwordSize + password + \0
	msgBuff[0] = to_char(sendPacketId::sendLoginData);

	msgBuff[1] = to_char(strlen(account) / 10);
	msgBuff[2] = to_char(strlen(account) % 10);

	strcat_s(msgBuff, 106, account);

	msgBuff[strlen(account) + 3] = to_char(strlen(password) / 10);
	msgBuff[strlen(account) + 4] = to_char(strlen(password) % 10);
	msgBuff[strlen(account) + 5] = '\0';

	strcat_s(msgBuff, 106, password);

	msgBuff[strlen(account) + strlen(password) + 6] = '\0';

	msgLen = strlen(msgBuff);
}


GetChatRoomListPacket::GetChatRoomListPacket()
{
	msgBuff = new char[GET_CHAT_ROOM_LIST_PACKET_SIZE] { 0 };

	msgBuff[0] = to_char(sendPacketId::getChatRoomList);
	msgBuff[1] = '\0';

	msgLen = strlen(msgBuff);
}
#pragma endregion


#pragma region ReceivePacket
ReceivePacket::ReceivePacket()
{
	id = receivePacketId::successOrFailure;
	msgBuff = new char[BUFF_SIZE] { 0 };
	msgLen = BUFF_SIZE;
}
#pragma endregion
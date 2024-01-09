#include "Packet.h"


#pragma region ClientPacket
ClientPacket::ClientPacket()
{
	id = clientPacketId::send_login_data;
	msgBuff = new char[0];
	msgLen = 0;
}
ClientPacket::~ClientPacket()
{
	delete msgBuff;
	msgBuff = nullptr;
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
	msgBuff[buffIndex++] = to_char(id);

	msgBuff[buffIndex++] = to_char((account.length() + password.length() + 4) / 10);
	msgBuff[buffIndex++] = to_char((account.length() + password.length() + 4) % 10);

	msgBuff[buffIndex++] = to_char(account.length() / 10);
	msgBuff[buffIndex++] = to_char(account.length() % 10);
	msgBuff[buffIndex] = '\0';

	strcat(msgBuff, account.c_str());
	buffIndex += account.length();

	msgBuff[buffIndex++] = to_char(password.length() / 10);
	msgBuff[buffIndex++] = to_char(password.length() % 10);
	msgBuff[buffIndex] = '\0';

	strcat(msgBuff, password.c_str());
	buffIndex += password.length();
	msgBuff[buffIndex] = '\0';

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


SendSignUpDataPacket::SendSignUpDataPacket()
{
	id = clientPacketId::send_sign_up_data;
}
SendSignUpDataPacket::SendSignUpDataPacket(string account, string password)
{
	id = clientPacketId::send_sign_up_data;
	this->account = account;
	this->password = password;

	int buffIndex = 0;
	msgBuff = new char[BUFF_SIZE] { 0 };

	//packetId(1) + totoalSize(2) + accountSize(2) + account + passwordSize(2) + password + \0
	msgBuff[buffIndex++] = to_char(id);

	msgBuff[buffIndex++] = to_char((account.length() + password.length() + 4) / 10);
	msgBuff[buffIndex++] = to_char((account.length() + password.length() + 4) % 10);

	msgBuff[buffIndex++] = to_char(account.length() / 10);
	msgBuff[buffIndex++] = to_char(account.length() % 10);
	msgBuff[buffIndex] = '\0';

	strcat(msgBuff, account.c_str());
	buffIndex += account.length();

	msgBuff[buffIndex++] = to_char(password.length() / 10);
	msgBuff[buffIndex++] = to_char(password.length() % 10);
	msgBuff[buffIndex] = '\0';

	strcat(msgBuff, password.c_str());
	buffIndex += password.length();
	msgBuff[buffIndex] = '\0';

	msgLen = strlen(msgBuff);
}


GetChatRoomListPacket::GetChatRoomListPacket()
{
	id = clientPacketId::get_chat_room_list;

	int buffIndex = 0;
	msgBuff = new char[BUFF_SIZE] { 0 };

	msgBuff[buffIndex++] = to_char(id);
	msgBuff[buffIndex] = '\0';

	msgLen = strlen(msgBuff);
}


SelectChatRoomPacket::SelectChatRoomPacket()
{
	id = clientPacketId::select_chat_room;
	chatRoomName = "";
}
SelectChatRoomPacket::SelectChatRoomPacket(string chatRoomName)
{
	id = clientPacketId::select_chat_room;
	this->chatRoomName = chatRoomName;

	int buffIndex = 0;
	msgBuff = new char[BUFF_SIZE] { 0 };

	//packetId(1) + totoalSize(2) + chatRoomNameSize(2) + chatRoomName + \0
	msgBuff[buffIndex++] = to_char(id);

	msgBuff[buffIndex++] = to_char((chatRoomName.length() + 2) / 10);
	msgBuff[buffIndex++] = to_char((chatRoomName.length() + 2) % 10);

	msgBuff[buffIndex++] = to_char(chatRoomName.length() / 10);
	msgBuff[buffIndex++] = to_char(chatRoomName.length() % 10);
	msgBuff[buffIndex] = '\0';

	strcat(msgBuff, chatRoomName.c_str());
	buffIndex += chatRoomName.length();
	msgBuff[buffIndex] = '\0';

	msgLen = strlen(msgBuff);
}
void SelectChatRoomPacket::setData(char* msgBuff, int msgLen)
{
	ClientPacket::setData(msgBuff, msgLen);

	int hasRead = 0;

	int chatRoomNameLen = to_int(msgBuff + hasRead, 2);
	hasRead += 2;
	chatRoomName.assign(msgBuff + hasRead, msgBuff + hasRead + chatRoomNameLen);
	hasRead += chatRoomNameLen;
}


CreateChatRoomPacket::CreateChatRoomPacket()
{
	id = clientPacketId::create_chat_room;
	chatRoomName = "";
}
CreateChatRoomPacket::CreateChatRoomPacket(string chatRoomName)
{
	id = clientPacketId::create_chat_room;
	this->chatRoomName = chatRoomName;

	int buffIndex = 0;
	msgBuff = new char[BUFF_SIZE] { 0 };

	//packetId(1) + totoalSize(2) + chatRoomNameSize(2) + chatRoomName + \0
	msgBuff[buffIndex++] = to_char(id);

	msgBuff[buffIndex++] = to_char((chatRoomName.length() + 2) / 10);
	msgBuff[buffIndex++] = to_char((chatRoomName.length() + 2) % 10);

	msgBuff[buffIndex++] = to_char(chatRoomName.length() / 10);
	msgBuff[buffIndex++] = to_char(chatRoomName.length() % 10);
	msgBuff[buffIndex] = '\0';

	strcat(msgBuff, chatRoomName.c_str());
	buffIndex += chatRoomName.length();
	msgBuff[buffIndex] = '\0';

	msgLen = strlen(msgBuff);
}


GetChatRoomMsgPacket::GetChatRoomMsgPacket()
{
	id = clientPacketId::get_chat_room_message;
	nowChatRoomMsgListSize = 0;
}
GetChatRoomMsgPacket::GetChatRoomMsgPacket(int nowChatRoomMsgListSize)
{
	id = clientPacketId::get_chat_room_message;
	this->nowChatRoomMsgListSize = nowChatRoomMsgListSize;

	int buffIndex = 0;
	msgBuff = new char[BUFF_SIZE] { 0 };

	//packetId(1) + totoalSize(2) + nowChatRoomMsgListSize(4) + \0
	msgBuff[buffIndex++] = to_char(id);

	msgBuff[buffIndex++] = '0';
	msgBuff[buffIndex++] = '4';

	msgBuff[buffIndex++] = to_char(nowChatRoomMsgListSize / 1000);
	msgBuff[buffIndex++] = to_char(nowChatRoomMsgListSize / 100 % 10);
	msgBuff[buffIndex++] = to_char(nowChatRoomMsgListSize / 10 % 10);
	msgBuff[buffIndex++] = to_char(nowChatRoomMsgListSize % 10);
	msgBuff[buffIndex] = '\0';

	msgLen = strlen(msgBuff);
}
void GetChatRoomMsgPacket::setData(char* msgBuff, int msgLen)
{
	ClientPacket::setData(msgBuff, msgLen);

	nowChatRoomMsgListSize = to_int(msgBuff, 4);
}


SendChatRoomMsgPacket::SendChatRoomMsgPacket()
{
	id = clientPacketId::send_chat_room_message;
	chatRoomMsg = ChatRoomMsg();
}
SendChatRoomMsgPacket::SendChatRoomMsgPacket(string sender, string chatRoomMsg)
{
	id = clientPacketId::send_chat_room_message;
	this->chatRoomMsg = ChatRoomMsg(sender, chatRoomMsg);

	int buffIndex = 0;
	msgBuff = new char[BUFF_SIZE] { 0 };

	//packetId(1) + totoalSize(2) + senderSize(2) + sender + chatRoomMsgSize(2) + chatRoomMsg + \0
	msgBuff[buffIndex++] = to_char(id);

	msgBuff[buffIndex++] = to_char((sender.length() + chatRoomMsg.length() + 4) / 10);
	msgBuff[buffIndex++] = to_char((sender.length() + chatRoomMsg.length() + 4) % 10);

	msgBuff[buffIndex++] = to_char(sender.length() / 10);
	msgBuff[buffIndex++] = to_char(sender.length() % 10);
	msgBuff[buffIndex] = '\0';

	strcat(msgBuff + buffIndex, sender.c_str());
	buffIndex += sender.length();

	msgBuff[buffIndex++] = to_char(chatRoomMsg.length() / 10);
	msgBuff[buffIndex++] = to_char(chatRoomMsg.length() % 10);
	msgBuff[buffIndex] = '\0';

	strcat(msgBuff + buffIndex, chatRoomMsg.c_str());
	buffIndex += chatRoomMsg.length();

	msgBuff[buffIndex] = '\0';

	msgLen = strlen(msgBuff);
}
void SendChatRoomMsgPacket::setData(char* msgBuff, int msgLen)
{
	ClientPacket::setData(msgBuff, msgLen);

	int senderLen, chatRoomMsgLen;
	int hasRead = 0;

	senderLen = to_int(msgBuff + hasRead, 2);
	hasRead += 2;

	chatRoomMsg.sender.assign(msgBuff + hasRead, msgBuff + hasRead + senderLen);
	hasRead += senderLen;

	chatRoomMsgLen = to_int(msgBuff + hasRead, 2);
	hasRead += 2;

	chatRoomMsg.msg.assign(msgBuff + hasRead, msgBuff + hasRead + chatRoomMsgLen);
	hasRead += chatRoomMsgLen;
}

#pragma endregion


//------------------------------------------------------------------------------------------------------------


#pragma region ServerPacket
ServerPacket::ServerPacket()
{
	id = serverPacketId::receive_success_or_failure;
	msgBuff = new char[BUFF_SIZE] { 0 };
	msgLen = BUFF_SIZE;
}
ServerPacket::~ServerPacket()
{
	delete msgBuff;
	msgBuff = nullptr;
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
ChatRoomListPacket::ChatRoomListPacket(vector<ChatRoom>& chatRoomList)
{
	id = serverPacketId::receive_chat_room_list;
	this->chatRoomList = chatRoomList;

	int buffIndex = 0;
	msgBuff = new char[BUFF_SIZE] { 0 };

	//packetId(1) + totoalSize(4) + chatRoomId(2) + chatRoomNameSize(2) + chatRoomName + ... + \0
	msgBuff[buffIndex++] = to_char(id);

	int totalSizeIndex = buffIndex;
	buffIndex += 4;
	int totalSize = 0;

	msgBuff[buffIndex] = '\0';

	for (ChatRoom chatRoom : chatRoomList)
	{
		msgBuff[buffIndex++] = to_char(chatRoom.id / 10);
		msgBuff[buffIndex++] = to_char(chatRoom.id % 10);

		msgBuff[buffIndex++] = to_char(chatRoom.name.length() / 10);
		msgBuff[buffIndex++] = to_char(chatRoom.name.length() % 10);
		msgBuff[buffIndex] = '\0';

		strcat(msgBuff + buffIndex, chatRoom.name.c_str());
		buffIndex += chatRoom.name.length();
		msgBuff[buffIndex] = '\0';

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


ChatRoomMsgPacket::ChatRoomMsgPacket()
{
	id = serverPacketId::receive_chat_room_message;
	chatRoomMsgList = vector<ChatRoomMsg>();
}
ChatRoomMsgPacket::ChatRoomMsgPacket(vector<ChatRoomMsg>& chatRoomMsgList)
{
	id = serverPacketId::receive_chat_room_message;
	this->chatRoomMsgList = chatRoomMsgList;

	int buffIndex = 0;
	msgBuff = new char[CHAT_ROOM_MSG_BUFF_SIZE] { 0 };

	//packetId(1) + totoalSize(4) + senderSize(2) + sender + chatRoomMsgSize(2) + chatRoomMsg + ... + \0
	msgBuff[buffIndex++] = to_char(id);

	int totalSizeIndex = buffIndex;
	buffIndex += 4;
	int totalSize = 0;

	msgBuff[buffIndex] = '\0';

	for (ChatRoomMsg msg : chatRoomMsgList)
	{
		msgBuff[buffIndex++] = to_char(msg.sender.length() / 10);
		msgBuff[buffIndex++] = to_char(msg.sender.length() % 10);
		msgBuff[buffIndex] = '\0';

		strcat(msgBuff + buffIndex, msg.sender.c_str());
		buffIndex += msg.sender.length();

		msgBuff[buffIndex++] = to_char(msg.msg.length() / 10);
		msgBuff[buffIndex++] = to_char(msg.msg.length() % 10);
		msgBuff[buffIndex] = '\0';

		strcat(msgBuff + buffIndex, msg.msg.c_str());
		buffIndex += msg.msg.length();

		msgBuff[buffIndex] = '\0';

		totalSize += 4 + msg.sender.length() + msg.msg.length();
	}

	msgBuff[totalSizeIndex++] = to_char(totalSize / 1000);
	msgBuff[totalSizeIndex++] = to_char(totalSize / 100 % 10);
	msgBuff[totalSizeIndex++] = to_char(totalSize / 10 % 10);
	msgBuff[totalSizeIndex++] = to_char(totalSize % 10);

	msgLen = strlen(msgBuff);
}
int ChatRoomMsgPacket::getTotalSizeLength()
{
	return 4;
}
void ChatRoomMsgPacket::setData(char* msgBuff, int msgLen)
{
	ServerPacket::setData(msgBuff, msgLen);

	int hasRead = 0;

	int senderLen, chatRoomMsgLen;
	string sender, chatRoomMsg;
	while (hasRead < msgLen)
	{
		senderLen = to_int(msgBuff + hasRead, 2);
		hasRead += 2;

		sender.assign(msgBuff + hasRead, msgBuff + hasRead + senderLen);
		hasRead += senderLen;

		chatRoomMsgLen = to_int(msgBuff + hasRead, 2);
		hasRead += 2;

		chatRoomMsg.assign(msgBuff + hasRead, msgBuff + hasRead + chatRoomMsgLen);
		hasRead += chatRoomMsgLen;

		chatRoomMsgList.emplace_back(sender, chatRoomMsg);
	}
}
#pragma endregion
#include "ChatRoom.h"


#pragma region TypeConversion
char to_char(int n)
{
	return '0' + n;
}

int to_int(const char* c, int cLen)
{
	int int_c = 0;

	for (int i = 0; i < cLen; i++)
	{
		int_c *= 10;
		int_c += c[i] - '0';
	}

	return int_c;
}
#pragma endregion

ChatRoom::ChatRoom()
{
	id = -1;
	name = "";
}
ChatRoom::ChatRoom(int id, string name)
{
	this->id = id;
	this->name = name;
}

ChatRoomMsg::ChatRoomMsg()
{
	this->sender = "";
	this->msg = "";
}
ChatRoomMsg::ChatRoomMsg(string sender, string msg)
{
	this->sender = sender;
	this->msg = msg;
}
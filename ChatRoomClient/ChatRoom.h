#ifndef CHATROOM_HEADER
#define CHATROOM_HEADER

#include <string>

using namespace std;


#define CHAT_BOT_API_KEY "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"


#define SERVER_IP "202.5.254.88"
#define SERVER_PORT 8080

#define ACCEPT_WAIT_MS 1000
#define RESTART_WAIT_MS 5000
#define GET_CHAT_ROOM_LIST_WAIT_MS 5000
#define GET_CHAT_ROOM_MSG_WAIT_MS 2500

#define BUFF_SIZE 1024
#define CHAT_ROOM_MSG_BUFF_SIZE 8192

#define NORMAL_TOTAL_SIZE_LENGTH 2
#define GET_CHAT_ROOM_LIST_TOTAL_SIZE_LENGTH 4
#define ACCOUNT_SIZE 20
#define PASSWORD_SIZE 20



enum clientPacketId
{
	send_login_data,
	send_sign_up_data,
	get_chat_room_list,
	select_chat_room,
	create_chat_room,
	get_chat_room_message,
	send_chat_room_message
};
enum serverPacketId
{
	receive_success_or_failure,
	receive_chat_room_list,
	receive_chat_room_message
};

enum clientStatus
{
	null_status,
	initializing,
	connecting,
	logging_in,
	chat_room_list,
	chat_room
};


class ChatRoom
{
public:
	int id;
	string name;

	ChatRoom();
	ChatRoom(int, string);
};

class ChatRoomMsg
{
public:
	string sender, msg;

	ChatRoomMsg();
	ChatRoomMsg(string, string);
};


#pragma region TypeConversion
char to_char(int);

int to_int(const char*, int);
#pragma endregion

#endif
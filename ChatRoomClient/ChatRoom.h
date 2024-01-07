#ifndef CHATROOM_HEADER
#define CHATROOM_HEADER

#include <string>

using namespace std;



#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9909

#define RESTART_WAIT_MS 5000

#define BUFF_SIZE 1024

#define NORMAL_TOTAL_SIZE_LENGTH 2
#define GET_CHAT_ROOM_LIST_TOTAL_SIZE_LENGTH 4
#define ACCOUNT_SIZE 20
#define PASSWORD_SIZE 20



enum clientPacketId
{
	send_login_data,
	get_chat_room_list,
	select_chat_room,
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


#pragma region TypeConversion
char to_char(int);

int to_int(const char*, int);
#pragma endregion

#endif
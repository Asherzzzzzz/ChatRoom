#ifndef CHATROOM_HEADER
#define CHATROOM_HEADER

#include <string>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9909

#define RESTART_WAIT_MS 5000

#define BUFF_SIZE 1024

#define ACCOUNT_SIZE 20
#define PASSWORD_SIZE 20



enum clientPacketId
{
	sendLoginData,
	getChatRoomList,
	sendChatRoomMessage
};
enum serverPacketId
{
	receiveSuccessOrFailure,
	receiveChatRoomList,
	receiveChatRoomMessage
};

enum clientStatus
{
	initializing,
	connecting,
	loggingIn,
	chatRoomList,
	chatRoom
};


struct ChatRoom
{
	int id = -1;
	std::string name = "";
};


#pragma region TypeConversion
char to_char(int);

int to_int(const char*, int);
#pragma endregion

#endif
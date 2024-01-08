//this is for linux os



//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <unistd.h>
//
//#include <stdlib.h>
//#include <iostream>
//#include <map>
//#include <vector>
//#include <thread>
//#include <mutex>
//#include <shared_mutex>
//#include <sqlite3.h>
//
//
//using namespace std;
//
//
//#include "ChatRoom.h"
//#include "Packet.h"
//
//
//#define  LISTEN_CLIENT_MAXIMUM 100
//
//
//#pragma region DatabaseFunctions
//shared_mutex chatRoomDatabase_mutex;
//bool modifyDatabase(string sql)
//{
//	sqlite3* db;
//	int return_value = sqlite3_open("ChatRoomDB.db", &db);
//	if (return_value != SQLITE_OK)
//	{
//		cerr << "Unable to open ChatRoomDB.db database: " << sqlite3_errmsg(db) << endl;
//		return false;
//	}
//
//	{
//		unique_lock<shared_mutex> lock(chatRoomDatabase_mutex);
//
//		return_value = sqlite3_exec(db, sql.c_str(), 0, 0, 0);
//		if (return_value != SQLITE_OK)
//		{
//			cerr << "modifyDatabase() error: " << sqlite3_errmsg(db) << endl;
//			return false;
//		}
//	}
//
//	sqlite3_close(db);
//	return true;
//}
//bool modifyDatabase(string sql, bool* hasModified)
//{
//	sqlite3* db;
//	int return_value = sqlite3_open("ChatRoomDB.db", &db);
//	if (return_value != SQLITE_OK)
//	{
//		cerr << "Unable to open ChatRoomDB.db database: " << sqlite3_errmsg(db) << endl;
//		return false;
//	}
//
//	{
//		unique_lock<shared_mutex> lock(chatRoomDatabase_mutex);
//
//		return_value = sqlite3_exec(db, sql.c_str(), 0, 0, 0);
//		if (return_value != SQLITE_OK)
//		{
//			cerr << "modifyDatabase() error: " << sqlite3_errmsg(db) << endl;
//			return false;
//		}
//
//		*hasModified = sqlite3_changes(db) > 0;
//	}
//
//	sqlite3_close(db);
//	return true;
//}
//bool checkDataExist(string sql, bool* dataExist)
//{
//	sqlite3* db;
//	int return_value = sqlite3_open("ChatRoomDB.db", &db);
//	if (return_value != SQLITE_OK)
//	{
//		cerr << "Unable to open ChatRoomDB.db database: " << sqlite3_errmsg(db) << endl;
//		return false;
//	}
//
//	sqlite3_stmt* statement;
//
//	{
//		shared_lock<shared_mutex> lock(chatRoomDatabase_mutex);
//
//		if (sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, NULL) != SQLITE_OK)
//		{
//			cerr << "getChatRoomListFromDatabase() error:" << sqlite3_errmsg(db) << endl;
//			return false;
//		}
//	}
//
//	*dataExist = false;
//	while (sqlite3_step(statement) == SQLITE_ROW)
//	{
//		*dataExist = true;
//	}
//	sqlite3_finalize(statement);
//
//	sqlite3_close(db);
//	return true;
//}
//bool getChatRoomListFromDatabase(vector<ChatRoom>* chatRoomList)
//{
//	sqlite3* db;
//	int return_value = sqlite3_open("ChatRoomDB.db", &db);
//	if (return_value != SQLITE_OK)
//	{
//		cerr << "Unable to open ChatRoomDB.db database: " << sqlite3_errmsg(db) << endl;
//		return false;
//	}
//
//	const char* sql = "SELECT id, chatRoomName FROM ChatRoom";
//	sqlite3_stmt* statement;
//
//	{
//		shared_lock<shared_mutex> lock(chatRoomDatabase_mutex);
//
//		if (sqlite3_prepare_v2(db, sql, -1, &statement, NULL) != SQLITE_OK)
//		{
//			cerr << "getChatRoomListFromDatabase() error:" << sqlite3_errmsg(db) << endl;
//			return false;
//		}
//	}
//
//	while (sqlite3_step(statement) == SQLITE_ROW)
//	{
//		chatRoomList->emplace_back(sqlite3_column_int(statement, 0)
//			, reinterpret_cast<const char*>(sqlite3_column_text(statement, 1)));
//	}
//	sqlite3_finalize(statement);
//
//	sqlite3_close(db);
//	return true;
//}
//bool getChatRoomMsgFromDatabase(string chatRoomName, vector<ChatRoomMsg>* chatRoomMsgList)
//{
//	sqlite3* db;
//	int return_value = sqlite3_open("ChatRoomDB.db", &db);
//	if (return_value != SQLITE_OK)
//	{
//		cerr << "Unable to open ChatRoomDB.db database: " << sqlite3_errmsg(db) << endl;
//		return false;
//	}
//
//	string str = "SELECT account, message FROM Message WHERE chatRoomName = '" + chatRoomName + "'";
//	const char* sql = str.c_str();
//	sqlite3_stmt* statement;
//
//	{
//		shared_lock<shared_mutex> lock(chatRoomDatabase_mutex);
//
//		if (sqlite3_prepare_v2(db, sql, -1, &statement, NULL) != SQLITE_OK)
//		{
//			cerr << "getChatRoomMsgFromDatabase() error:" << sqlite3_errmsg(db) << endl;
//			return false;
//		}
//	}
//
//	while (sqlite3_step(statement) == SQLITE_ROW) {
//		chatRoomMsgList->emplace_back(reinterpret_cast<const char*>(sqlite3_column_text(statement, 0))
//			, reinterpret_cast<const char*>(sqlite3_column_text(statement, 1)));
//	}
//	sqlite3_finalize(statement);
//
//	sqlite3_close(db);
//	return true;
//}
//#pragma endregion
//
//
//#pragma region Client&Server
//class Client
//{
//public:
//	int socket;
//	char* ip;
//	string account, chatRoomName;
//	clientStatus status;
//
//private:
//	string password;
//
//public:
//	Client()
//	{
//		socket = -1;
//		ip = new char[INET_ADDRSTRLEN] { 0 };
//		account = "";
//		password = "";
//		chatRoomName = "";
//		status = clientStatus::logging_in;
//	}
//	Client(int socket, char* ip)
//	{
//		this->socket = socket;
//		this->ip = ip;
//		account = "";
//		password = "";
//		chatRoomName = "";
//		status = clientStatus::logging_in;
//	}
//
//	void setAccountAndPassword(string account, string password)
//	{
//		this->account = account;
//		this->password = password;
//	}
//	void setChatRoomName(string chatRoomName)
//	{
//		this->chatRoomName = chatRoomName;
//	}
//};
//bool equal(Client& c1, Client& c2)
//{
//	return c1.socket == c2.socket;
//}
//
//
//class Server
//{
//public:
//	static Server* instance;
//	int socket;
//	vector<Client> clientList;
//	vector<thread> clientThreadList;
//
//private:
//	map<string, string> accountAndPassword;
//
//	shared_mutex accountAndPassword_mutex, chatRoomList_mutex;
//
//private:
//	Server()
//	{
//		socket = -1;
//		clientList = vector<Client>();
//		clientThreadList = vector<thread>();
//
//		accountAndPassword = map<string, string>();
//	}
//
//public:
//	static Server& getInstance()
//	{
//		return *instance;
//	}
//
//	bool addNewAccountAndPassword(string account, string password)
//	{
//		unique_lock<shared_mutex> lock(accountAndPassword_mutex);
//
//		bool exist, hasModified;
//		if (!checkDataExist("SELECT id FROM Account \
//						WHERE account = '" + account + "'"
//			, &exist))
//			return false;
//
//		if (exist)
//			return false;
//
//		if (!modifyDatabase("INSERT INTO Account (account, password)\
//						VALUES ('" + account + "', '" + password + "')"
//			, &hasModified))
//			return false;
//
//		return hasModified;
//	}
//	bool checkAccountAndPassword(string account, string password)
//	{
//		shared_lock<shared_mutex> lock(accountAndPassword_mutex);
//
//		bool match;
//		if (!checkDataExist("SELECT id FROM Account \
//						WHERE account = '" + account + "' AND password = '" + password + "'"
//			, &match))
//			return false;
//
//		return match;
//	}
//
//	bool addNewChatRoom(string chatRoomName)
//	{
//		// check same name has already existed
//		bool hasModified;
//		if (!modifyDatabase("INSERT INTO ChatRoom (chatRoomName)\
//						VALUES ('" + chatRoomName + "')"
//			, &hasModified))
//			return false;
//
//		return hasModified;
//	}
//	bool joinChatRoom(Client* client, string chatRoomName)
//	{
//		bool hasModified;
//		if (!modifyDatabase("UPDATE Member SET chatRoomName='" + chatRoomName
//			+ "' WHERE account='" + client->account + "'"
//			, &hasModified))
//			return false;
//
//		if (hasModified)
//			return true;
//
//		if (!modifyDatabase("INSERT INTO Member (account, chatRoomName)\
//						VALUES ('" + client->account + "', '" + chatRoomName + "')"
//			, &hasModified))
//			return false;
//
//		return hasModified;
//	}
//	bool quitChatRoom(Client* client, string chatRoomName)
//	{
//
//		return true;
//	}
//
//	bool sendChatRoomMsg(Client* client, ChatRoomMsg chatRoomMsg)
//	{
//		bool hasModified;
//		if (!modifyDatabase("INSERT INTO Message (account, message, chatRoomName) \
//						VALUES ('" + chatRoomMsg.sender + "', '" + chatRoomMsg.msg + "', '" + client->chatRoomName + "')"
//			, &hasModified))
//			return false;
//
//		return hasModified;
//	}
//};
//Server* Server::instance = new Server();
//Server& serverInstance = Server::getInstance();
//#pragma endregion
//
//
//#pragma region ErrorPrint
//void restartServerErrorPrint(string errorFunctionName)
//{
//	cerr << errorFunctionName << "() failed with error" << endl;
//	cerr << "Restarting..." << endl;
//	close(serverInstance.socket);
//	sleep(RESTART_WAIT_MS / 1000);
//}
//void reconnectClientErrorPrint(string errorFunctionName, Client* client)
//{
//	if (client->status == clientStatus::null_status)
//		return;
//
//	cerr << "When processing " << client->account;
//	if (client->ip)
//		cerr << "-" << client->ip;
//	cerr << ", " << errorFunctionName << "() failed with error" << endl;
//
//	client->status = clientStatus::null_status;
//}
//#pragma endregion
//
//
//#pragma region Send&Receive
//bool sendToClient(Client* client, ServerPacket packet)
//{
//	int hasSent = 0, nowSend = 0;
//	do
//	{
//		nowSend = send(client->socket, packet.msgBuff + hasSent, packet.msgLen - hasSent, 0);
//		if (nowSend < 0)
//		{
//			reconnectClientErrorPrint("send", client);
//			return false;
//		}
//		hasSent += nowSend;
//	} while (hasSent != packet.msgLen);
//
//	return true;
//}
//
//bool receivePacketIdFromClient(Client* client, clientPacketId* packetId)
//{
//	char* buff = new char[1] { 0 };
//
//	int hasReceived = recv(client->socket, buff, 1, 0);
//	if (hasReceived < 0)
//	{
//		reconnectClientErrorPrint("recv", client);
//		return false;
//	}
//
//	*packetId = (clientPacketId)to_int(buff, 1);
//
//	return true;
//}
//
//bool receiveMsgFromClient(Client* client, ClientPacket* packet)
//{
//	char* buff = new char[2] { 0 };
//
//	// total length section
//	int hasReceived = recv(client->socket, buff, 2, 0);
//	if (hasReceived < 0)
//	{
//		reconnectClientErrorPrint("recv", client);
//		return false;
//	}
//
//	int msgLen = to_int(buff, 2);
//
//	// msg section
//	int nowReceive = 0;
//	hasReceived = 0;
//	char* msgBuff = new char[msgLen] { 0 };
//	do
//	{
//		nowReceive = recv(client->socket, msgBuff + hasReceived, msgLen - hasReceived, 0);
//		if (nowReceive < 0)
//		{
//			reconnectClientErrorPrint("recv", client);
//			return false;
//		}
//		hasReceived += nowReceive;
//	} while (hasReceived < msgLen);
//
//	packet->setData(msgBuff, msgLen);
//
//	return true;
//}
//#pragma endregion
//
//
//#pragma region Initialization
//bool initializeDatabase()
//{
//	if (!modifyDatabase("CREATE TABLE IF NOT EXISTS Account (\
//						id INTEGER PRIMARY KEY AUTOINCREMENT,\
//						account TEXT NOT NULL,\
//						password TEXT NOT NULL\
//					);"))
//		return false;
//
//	if (!modifyDatabase("CREATE TABLE IF NOT EXISTS ChatRoom (\
//						id INTEGER PRIMARY KEY AUTOINCREMENT,\
//						chatRoomName TEXT NOT NULL\
//					);"))
//		return false;
//
//	if (!modifyDatabase("CREATE TABLE IF NOT EXISTS Member (\
//				id INTEGER PRIMARY KEY,\
//				account TEXT NOT NULL,\
//				chatRoomName TEXT NOT NULL\
//			);"))
//		return false;
//
//	if (!modifyDatabase("CREATE TABLE IF NOT EXISTS Message (\
//				id INTEGER PRIMARY KEY,\
//				account TEXT NOT NULL,\
//				message TEXT,\
//				chatRoomName TEXT NOT NULL\
//			);"))
//		return false;
//
//	return true;
//}
//
//bool initializeSocket()
//{
//	serverInstance.socket = socket(AF_INET, SOCK_STREAM, 0);
//	if (serverInstance.socket == -1)
//	{
//		cerr << "socket() failed with error" << endl;
//		return false;
//	}
//
//	return true;
//}
//
//bool setSocketOption()
//{
//	int opt = 1;
//	int return_val = setsockopt(serverInstance.socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
//	if (serverInstance.socket == -1)
//	{
//		restartServerErrorPrint("setsockopt");
//		return false;
//	}
//
//	return true;
//}
//
//bool bindSocket()
//{
//	struct sockaddr_in bindingAddr;// = { AF_INET, htons(SERVER_PORT), INADDR_ANY };
//	bindingAddr.sin_family = AF_INET;
//	inet_aton("0.0.0.0", &bindingAddr.sin_addr);
//	bindingAddr.sin_port = htons(SERVER_PORT);
//	//memset(&bindingAddr.sin_zero, 0, 8);
//
//	int return_value = bind(serverInstance.socket, (sockaddr*)&bindingAddr, sizeof(bindingAddr));
//	if (return_value == -1)
//	{
//		restartServerErrorPrint("bind");
//		return false;
//	}
//
//	return true;
//}
//
//bool listenSocket()
//{
//	int return_value = listen(serverInstance.socket, LISTEN_CLIENT_MAXIMUM);
//
//	if (return_value == -1)
//	{
//		restartServerErrorPrint("listen");
//		return false;
//	}
//
//	cout << "Started listening to local port..." << endl;
//	return true;
//}
//#pragma endregion
//
//
//#pragma region ClientProcess
//void clientSignUp(Client* client)
//{
//	SendSignUpDataPacket sendSignUpDataPacket;
//	if (!receiveMsgFromClient(client, &sendSignUpDataPacket))
//	{
//		reconnectClientErrorPrint("receiveMsgFromClient", client);
//		return;
//	}
//
//	bool signUpSuccessOrFailure = serverInstance.addNewAccountAndPassword(sendSignUpDataPacket.account, sendSignUpDataPacket.password);
//
//
//	if (!signUpSuccessOrFailure)
//	{
//		cout << client->ip << " failed to sign up" << endl;
//	}
//	else
//	{
//		cout << client->account << "-" << client->ip << " successfully signed up" << endl;
//	}
//
//	SuccessOrFailurePacket successOrFailurePacket(signUpSuccessOrFailure);
//	if (!sendToClient(client, successOrFailurePacket))
//	{
//		reconnectClientErrorPrint("sendToClient", client);
//		return;
//	}
//}
//
//bool clientLogin(Client* client, bool* loginSuccessOrFailure)
//{
//	SendLoginDataPacket loginDataPacket;
//	if (!receiveMsgFromClient(client, &loginDataPacket))
//	{
//		reconnectClientErrorPrint("receiveMsgFromClient", client);
//		return false;
//	}
//
//	*loginSuccessOrFailure = serverInstance.checkAccountAndPassword(loginDataPacket.account, loginDataPacket.password);
//
//
//	if (!*loginSuccessOrFailure)
//	{
//		cout << client->ip << " failed to log in" << endl;
//	}
//	else
//	{
//		client->setAccountAndPassword(loginDataPacket.account, loginDataPacket.password);
//		cout << client->account << "-" << client->ip << " successfully logged in" << endl;
//	}
//
//	SuccessOrFailurePacket successOrFailurePacket(*loginSuccessOrFailure);
//	if (!sendToClient(client, successOrFailurePacket))
//	{
//		reconnectClientErrorPrint("sendToClient", client);
//		return false;
//	}
//
//	return true;
//}
//
//bool clientGetChatRoomList(Client* client)
//{
//	//string clientNowChatRoomName = "";
//	//getClientNowChatRoomFromDatabase(client->account, &clientNowChatRoomName)
//
//	vector<ChatRoom> chatRoomList;
//	if (!getChatRoomListFromDatabase(&chatRoomList))
//		return false;
//
//	ChatRoomListPacket chatRoomListPacket(&chatRoomList);
//	if (!sendToClient(client, chatRoomListPacket))
//	{
//		reconnectClientErrorPrint("sendToClient", client);
//		return false;
//	}
//	return true;
//}
//
//bool clientJoinChatRoom(Client* client, bool* JoinChatRoomSuccessOrFailure)
//{
//	SelectChatRoomPacket selectChatRoomPacket;
//	if (!receiveMsgFromClient(client, &selectChatRoomPacket))
//	{
//		reconnectClientErrorPrint("receiveMsgFromClient", client);
//		return false;
//	}
//
//	*JoinChatRoomSuccessOrFailure = serverInstance.joinChatRoom(client, selectChatRoomPacket.chatRoomName);
//
//	if (!*JoinChatRoomSuccessOrFailure)
//	{
//		cout << client->ip << " failed to join ChatRoom " << selectChatRoomPacket.chatRoomName << endl;
//	}
//	else
//	{
//		client->setChatRoomName(selectChatRoomPacket.chatRoomName);
//		cout << client->account << "-" << client->ip << " successfully joined ChatRoom " << selectChatRoomPacket.chatRoomName << endl;
//	}
//
//	SuccessOrFailurePacket successOrFailurePacket(*JoinChatRoomSuccessOrFailure);
//	if (!sendToClient(client, successOrFailurePacket))
//	{
//		reconnectClientErrorPrint("sendToClient", client);
//		return false;
//	}
//
//	return true;
//}
//
//
//void clientGetChatRoomMsg(Client* client)
//{
//	GetChatRoomMsgPacket getChatRoomMsgPacket;
//	if (!receiveMsgFromClient(client, &getChatRoomMsgPacket))
//	{
//		reconnectClientErrorPrint("receiveMsgFromClient", client);
//		return;
//	}
//
//	vector<ChatRoomMsg> chatRoomMsgList;
//	if (!getChatRoomMsgFromDatabase(client->chatRoomName, &chatRoomMsgList))
//		return;
//
//	ChatRoomMsgPacket chatRoomMsgPacket(&chatRoomMsgList);
//	if (!sendToClient(client, chatRoomMsgPacket))
//	{
//		reconnectClientErrorPrint("sendToClient", client);
//		return;
//	}
//}
//
//
//void clientSendChatRoomMsg(Client* client)
//{
//	SendChatRoomMsgPacket sendChatRoomMsgPacket;
//	if (!receiveMsgFromClient(client, &sendChatRoomMsgPacket))
//	{
//		reconnectClientErrorPrint("receiveMsgFromClient", client);
//		return;
//	}
//
//	bool sendChatRoomMsgSuccessOrFailure = serverInstance.sendChatRoomMsg(client, sendChatRoomMsgPacket.chatRoomMsg);
//
//	SuccessOrFailurePacket successOrFailurePacket(sendChatRoomMsgSuccessOrFailure);
//	if (!sendToClient(client, successOrFailurePacket))
//	{
//		reconnectClientErrorPrint("sendToClient", client);
//		return;
//	}
//}
//
//
//void clientProcess(Client* client)
//{
//	clientPacketId packetId;
//
//	while (client)
//	{
//		if (client->status == clientStatus::null_status)
//		{
//			close(client->socket);
//			return;
//		}
//
//		if (!receivePacketIdFromClient(client, &packetId))
//		{
//			reconnectClientErrorPrint("receivePacketIdFromClient", client);
//			continue;
//		}
//
//		//check status and packet
//		switch (packetId)
//		{
//		case clientPacketId::send_sign_up_data:
//			clientSignUp(client);
//			break;
//
//		case clientPacketId::send_login_data:
//			bool loginSuccessOrFailure;
//
//			if (!clientLogin(client, &loginSuccessOrFailure))
//				break;
//
//			if (loginSuccessOrFailure)
//				client->status = clientStatus::chat_room_list;
//			break;
//
//		case clientPacketId::get_chat_room_list:
//			clientGetChatRoomList(client);
//			break;
//
//		case clientPacketId::select_chat_room:
//			bool JoinChatRoomSuccessOrFailure;
//
//			if (!clientJoinChatRoom(client, &JoinChatRoomSuccessOrFailure))
//				break;
//
//			if (JoinChatRoomSuccessOrFailure)
//				client->status = clientStatus::chat_room;
//			break;
//
//		case clientPacketId::get_chat_room_message:
//			clientGetChatRoomMsg(client);
//			break;
//
//		case clientPacketId::send_chat_room_message:
//			clientSendChatRoomMsg(client);
//			break;
//		}
//	}
//
//}
//#pragma endregion
//
//
//int main()
//{
//	//test
//	//modifyDatabase("INSERT INTO ChatRoom (chatRoomName)\
//					VALUES ('test');");
//	//test
//
//
//	if (!initializeDatabase())
//		return false;
//
//	while (true)
//	{
//		if (!initializeSocket())
//			continue;
//
//		if (!setSocketOption())
//			continue;
//
//		cout << "The socket is successfully initialized, socket id=" << serverInstance.socket << endl;
//
//		if (!bindSocket())
//			continue;
//
//		if (!listenSocket())
//			continue;
//
//		while (true)
//		{
//			int clientSocket;
//			struct sockaddr_in clientAddr;
//			socklen_t addrLen = sizeof(sockaddr_in);
//
//			clientSocket = accept(serverInstance.socket, (sockaddr*)&clientAddr, &addrLen);
//			if (clientSocket == -1)
//				continue;
//
//			cout << "Accept client socket : " << clientSocket << endl;
//
//			char* client_ip = new char[INET_ADDRSTRLEN] { 0 };
//			inet_ntop(AF_INET, &clientAddr, client_ip, INET_ADDRSTRLEN);
//
//			Client* client = new Client(clientSocket, client_ip);
//			thread client_thread(clientProcess, client);
//			client_thread.detach();
//		}
//	}
//
//	return 0;
//}
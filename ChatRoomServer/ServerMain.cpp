int opt = 1;
int return_val = setsockopt(clientSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
if (clientSocket == SOCKET_ERROR)
{
	cout << endl << "setsockopt() failed with error: " << WSAGetLastError() << endl;
	WSACleanup();
	exit(EXIT_FAILURE);
}
#include <iostream>
#include <WS2tcpip.h>
#include <string>
#pragma comment (lib, "ws2_32.lib")

using namespace std;


void main()
{
	string ipAddress = "127.0.0.1"; // ip of the server
	int port = 1901;				// listening port on server

	// init winSock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);

	int wsResult = WSAStartup(ver, &data);

	if (wsResult != 0)
	{
		cerr << "Can't start Winsock, Err #" << wsResult << endl;
		return;
	}


	// Create socket
	SOCKET sock_ = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_ == INVALID_SOCKET)
	{
		cerr << "Can't create socket, Err #" << WSAGetLastError;
		WSACleanup();
		return;
	}

	// Fill in a hint structure - hint structure tells winSock that what server and what port we want to connect to
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);


	// Connect to server
	int connRes = connect(sock_, (sockaddr*)&hint, sizeof(hint));
	if (connRes == SOCKET_ERROR)
	{
		cerr << "Can't connect to server, Err #" << WSAGetLastError() << endl;
		closesocket(sock_);
		WSACleanup();
		return;

	}

	// Do-While loop to send and receive data
	char buf[4096];
	string userInput;

	do
	{
		// Prompt the user for some text
		cout << "$>: ";
		getline(cin, userInput);

		if (userInput.size() > 0)  // Make sure the user has typed in something
		{
			// Send the text
			int sendRes = send(sock_, userInput.c_str(), userInput.size() + 1, 0);
			if (sendRes != SOCKET_ERROR)
			{
				// Wait for response
				ZeroMemory(buf, 4096);
				int byteRecv = recv(sock_, buf, 4096, 0);
				if (byteRecv > 0)
				{
					// Echo response to console
					cout << "SERVER replied> " << string(buf, 0, byteRecv) << endl;
				}


			}
		}
	} while (userInput.size() > 0);

	// close 
	closesocket(sock_);
	WSACleanup();
}

# include <iostream>

# include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;


void main()
{
	// Initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);

	if (wsOk != 0) {
		cerr << "Can't Init winsock! Quitting" << endl;
		return;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0); // in UNIX, socket() returns an int, Windows returns a type SOCKET
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		return;
	}


	// Bind the ip address and port to a socket 
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000); // network is big-endian and the computer is little-endian
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // bind to any address, Could also use inet_pton

	bind(listening, (sockaddr*)&hint, sizeof(hint)); // bind the socket to the ip and port to send and recieve connection


	// Tell winsock the socket is for listening
	listen(listening, SOMAXCONN);

	// Wait for connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize); // 

	// if accept() succeed

	char host[NI_MAXHOST]; // Client's remote name
	char service[NI_MAXHOST]; // Service (i.e. port) the clinet is connect on

	ZeroMemory(host, NI_MAXHOST); // like memset in UNIX
	ZeroMemory(host, NI_MAXHOST); // clean up garbage

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << " connected on port " << service << endl;
	} 
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " << ntohs(client.sin_port) << endl;
	}


	// Close listening socket
	closesocket(listening);


	// while loop: accept and echo message back to client
	char buf[4096];

	while (true)
	{
		ZeroMemory(buf, 4096); // should have more than 4k, read all bytes 

		// wait for client to send data
		int byteRecv = recv(clientSocket, buf, 4096, 0); // writing whatever it gets to buf

		if (byteRecv == SOCKET_ERROR)
		{
			cerr << "Error in recv(). Quitting" << endl;
			break;
		}
		if (byteRecv == 0)
		{
			cout << "Client disconnected" << endl;
			break;
		}

		// Display message
		cout << "from Client: " << string(buf, 0, byteRecv) << endl;

		// Echo message back to client
		send(clientSocket, buf, byteRecv + 1, 0);


	}


	// Close the socket
	closesocket(clientSocket);
	// Cleanup winsock
	WSACleanup();
	
}

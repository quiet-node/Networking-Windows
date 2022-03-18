# include <iostream>
# include <WS2tcpip.h>
# include <sstream>

#pragma comment (lib, "ws2_32.lib")

#define LISTENING_PORT 1901

void main()
{
	// Initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);

	if (wsOk != 0) {
		std::cerr << "Can't Init winsock! Quitting" << std::endl;
		return;
	}

	// Create a listening socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0); // in UNIX, socket() returns an int, winsock returns a type SOCKET
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "Can't create a socket! Quitting" << std::endl;
		return;
	}


	// Bind the ip address and port to a socket 
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(LISTENING_PORT); // network is big-endian and the computer is little-endian
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // bind to any address, Could also use inet_pton

	bind(listening, (sockaddr*)&hint, sizeof(hint)); // bind the socket to the ip and port to send and receive connection


	// Tell winsock the socket is for listening
	listen(listening, SOMAXCONN);

	
	// >>> MULTI-CLIENT <<<
	fd_set master; // select()::fd_set master is a set of 1 listening value and multiple client values 
	FD_ZERO(&master); // clear the master set

	// add listening to the master set
	FD_SET(listening, &master);
	std::cout << "Multi-clients TCP server..." << std::endl;

	// Create a running server to accept multiple clients
	while (true)
	{
		// make a copy of master set because the set can be destroyed after select() is called
		fd_set clonedMaster = master;

		/* 
			Learn more about select: https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-select
			The select function determines the status of one or more sockets, waiting if necessary, to perform synchronous I/O. 
		*/
		int socketCount = select(0, &clonedMaster, nullptr, nullptr, nullptr);  // if theres any sockets, it will be stored in the &clonedMaster set
		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock_ = clonedMaster.fd_array[i];
			if (sock_ == listening)
			{
				// Accept a new connection
				SOCKET client_= accept(listening, nullptr, nullptr);

				// Add the new connection to the list of connected clients (aster set)
				FD_SET(client_, &master); 

				// Send a welcome message to the connected client
				std::string welcomeMsg = "Welcome!";
				send(client_, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);

				
			}
			else
			{
				char buf[4096];
				ZeroMemory(buf, 4096);

				// Accept a new mesasge
				int bytesIn = recv(sock_, buf, 4096, 0);
				if (bytesIn <= 0) 
				{
					// Drop the clinet
					closesocket(sock_);
					FD_CLR(sock_, &master);
				}
				else
				{
					// Send msg to other clients, NOT listening socket
					for (int i = 0; i < master.fd_count; i++) {
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && outSock != sock_)
						{
							// if the socket at master[i] is not the server or itself, then it's one of the other clients
							// send message to that client i
							
							std::ostringstream ss;
							ss << "Socket #" << sock_ << ": " << buf << "\r\n";
							std::string strOut = ss.str();
							send(outSock, strOut.c_str(), strOut.size() + 1, 0);
						}
					}
				}

			}

		}
	}




	// clean up winsock
	WSACleanup();

	system("pause");

}
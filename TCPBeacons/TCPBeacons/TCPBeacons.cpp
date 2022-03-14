#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <string>

#pragma comment (lib, "Ws2_32.lib")


#define MAC_LENGTH 18
#define PORT_LENGTH 6
#define MAX_RECEIVE_BUFFER_SIZE 4096
// #define BROADCAST_PORT 21221
#define LISTENING_PORT 54000

int __cdecl main(int argc, char* argv[])
{
    // FILE HANDLER

    // init attributes
    std::string fileName;
    int innerDelay = 10;
    int outerDelay = 1000;
    std::string secondArg;
    std::string thirdArg;

    if (argc < 2) // if user doesn't specify filename => prompt them
    {
        std::cout << "Enter the path/filename to the beacon file: ";
        std::cin >> fileName;
        std::cout << "Parsing File: " + fileName << std::endl;
    }

    // After the file is inputed, parse the file => push it to a vector
    std::ifstream infile(fileName);
    std::string line;
    std::vector<std::string> beacons;
    while (std::getline(infile, line))
    {
        beacons.push_back(line); // push to beacons vector
    }

    for (int i = 0; beacons.size(); i++) {
        std::cout << beacons[i] << std::endl;
    }


    // TCP/IP HANDLER

    std::string ipAddress = "127.0.0.1"; // ip of the server

    // init winSock
    WSAData data;
    WORD ver = MAKEWORD(2, 2);

    int wsResult = WSAStartup(ver, &data);

    if (wsResult != 0)
    {
        std::cerr << "Can't start Winsock, Err #" << wsResult << std::endl;
        return;
    }


    // Create socket
    SOCKET sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ == INVALID_SOCKET)
    {
        std::cerr << "Can't create socket, Err #" << WSAGetLastError;
        WSACleanup();
        return;
    }

    // Fill in a outgoing structure - this structure tells winSock that what server and what port we want to connect to
    sockaddr_in outgoing;
    outgoing.sin_family = AF_INET;
    outgoing.sin_port = htons(LISTENING_PORT);
    inet_pton(AF_INET, ipAddress.c_str(), &outgoing.sin_addr);


    // Connect to server
    int connRes = connect(sock_, (sockaddr*)&outgoing, sizeof(outgoing));
    if (connRes == SOCKET_ERROR)
    {
        std::cerr << "Can't connect to server, Err #" << WSAGetLastError() << std::endl;
        closesocket(sock_);
        WSACleanup();
        return;

    }


    // SEND BEACONS TO SERVER

    std::string buffer = "n00:00:00:00:00:01n192.168.0.0n47.519961n10.698863n3050.078383";
    bool exit = false;
    std::cout << "Starting UDP Broadcaset. Press esc to exit! " << std::endl;
    char buf[4096];

    while (exit == false)
    {
        for (int i = 0; i < beacons.size(); i++)
        {
            int sendRes = sendto(sock_, beacons[i].c_str(), beacons[i].length() + 1, 0, (struct sockaddr*)&outgoing, sizeof(struct sockaddr_in));
            if (sendRes != SOCKET_ERROR)
            {
                std::cout << beacons[i] << std::endl;
                // Wait for response from server
                ZeroMemory(buf, 4096);
                int byteRecv = recv(sock_, buf, 4096, 0);
                if (byteRecv > 0)
                {
                    // Echo response to console
                    std::cout << "SERVER replied> " << std::string(buf, 0, byteRecv) << std::endl;
                }

                // exit when user hit esc
                if (GetAsyncKeyState(VK_ESCAPE)) {
                    exit = true;
                    break;
                }
                Sleep(innerDelay);
            }
        }

        Sleep(outerDelay);

    }
    std::cout << "Stopped TCP/IP Connection." << std::endl;
    closesocket(sock_);
    WSACleanup();
    
}


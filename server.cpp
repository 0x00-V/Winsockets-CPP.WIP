#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <iostream>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")
#define DEFAULT_PORT "55555"
#define DEFAULT_BUFLEN 512



int main()
{
    std::cout << "Server online!\n";
    WSADATA wsaData;
    int iResult, iSendResult, recvbuflen = DEFAULT_BUFLEN;
    char recvbuf[DEFAULT_BUFLEN];


    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;


    struct addrinfo *result = NULL, *ptr = NULL, hints;
    ZeroMemory(&hints, sizeof (hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;


    // Init Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(iResult != 0)
    {
        std::cout << "WSAStartup Failure\n" << iResult << std::endl;
        WSACleanup();
        return 1;
    }


    // Resolve to local addr and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if(iResult != 0)
    {   
        std::cout << "getaddrinfo failed: " << iResult << std::endl;
        WSACleanup();
        return 1;
    }

    
    // Listen for client connections
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if(ListenSocket == INVALID_SOCKET)
    {
        std::cout << "Socket err: " << WSAGetLastError();
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }


    // TCP Listen
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if(iResult == SOCKET_ERROR)
    {
        std::cout << "bind failed!\n" << WSAGetLastError();
        closesocket(ListenSocket);
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }


    if(listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cout << "Listen failure!\n" << WSAGetLastError();
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }


    ClientSocket = accept(ListenSocket, NULL, NULL);
    if(ClientSocket == INVALID_SOCKET)
    {
        std::cout << "Accept failed!\n" << WSAGetLastError();
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }


    // Send and receive data on sock
    do
    {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if(iResult > 0)
        {
            std::cout << "Bytes received: " << iResult << " \n";
            //Echo buffer to sender
            iSendResult = send(ClientSocket, recvbuf, iResult, 0);
            if(iSendResult == SOCKET_ERROR)
            {
                std::cout << "Failed to send\n" << WSAGetLastError();
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            std::cout << "Bytes sent: " << iSendResult << '\n';
        } else if(iResult == 0) std::cout << "Connection closing...\n";
        else 
        {
            std::cout << "recv failed: " << WSAGetLastError() << '\n';
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    } while (iResult > 0);

    iResult = shutdown(ClientSocket, SD_SEND);
    if(iResult == SOCKET_ERROR)
    {
        std::cout << "Failed to shutdown!\n" << WSAGetLastError();
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }


    // Sock cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return 0;

}


// Build: g++ server.cpp -o server -lws2_32
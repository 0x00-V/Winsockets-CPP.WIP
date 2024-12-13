#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <iostream>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")
#define DEFAULT_PORT "27015"

int main()
{
    WSADATA wsaData;

    int iResult;

    // Init Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(iResult != 0)
    {
        std::cout << "WSAStartup Failure\n" << iResult << std::endl;
        return 1;
    }

    
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    ZeroMemory(&hints, sizeof (hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve to local addr and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if(iResult != 0)
    {   
        std::cout << "getaddrinfo failed: " << iResult << std::endl;
        WSACleanup();
        return 1;
    }

    SOCKET ListenSocket = INVALID_SOCKET;


    // Listen for client connections
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if(ListenSocket == INVALID_SOCKET)
    {
        std::cout << "Socket err: " << WSAGetLastError();
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

}

https://learn.microsoft.com/en-us/windows/win32/winsock/binding-a-socket
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")
#define DEFAULT_PORT "55555"
#define DEFAULT_BUFLEN 512



int main(int argc, char* argv[])
{
    WSADATA wsaData;

    int iResult, recvbuflen = DEFAULT_BUFLEN;
    const char *sendbuf = "This is a test";
    char recvbuf[DEFAULT_BUFLEN];

    // Init Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(iResult != 0)
    {
        std::cout << "WSAStartup Failure\n" << iResult << std::endl;
        return 1;
    }

    struct addrinfo *result = NULL,
                *ptr = NULL,
                hints;

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve server addr and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    SOCKET ConnectSocket = INVALID_SOCKET;

    ptr=result;
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    if(ConnectSocket == INVALID_SOCKET)
    {
        std::cout << "Error at socket: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }


    // Connect to server
    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if(iResult == SOCKET_ERROR)
    {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }

    freeaddrinfo(result);
    if(ConnectSocket == INVALID_SOCKET)
    {
        std::cout << "Unable to connect to server!\n";
        WSACleanup();
        return 1;
    }

    // Send init buff
    iResult = send(ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
    if(iResult == SOCKET_ERROR)
    {
        std::cout << "Failed to send " << WSAGetLastError() << '\n';
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "Bytes sent: " << iResult << '\n';

    // Shutdown sending, receiving will still be possible
    iResult = shutdown(ConnectSocket, SD_SEND);
    if(iResult == SOCKET_ERROR)
    {
        std::cout << "Shutdown failure  :" << WSAGetLastError() << '\n';
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Receive until connection closes
    do 
    {
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if(iResult > 0) std::cout << "Bytes received: " << iResult << '\n';
        else if (iResult == 0) std::cout << "Connection closed....\n";
        else std::cout << "recv failure: " << WSAGetLastError() << '\n';
    } while(iResult > 0);

    closesocket(ConnectSocket);
    WSACleanup();
    return 0;
}
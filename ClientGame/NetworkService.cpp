#include "stdafx.h"
#include "NetworkService.h"


NetworkService::NetworkService()
{
	InitializeWSA();
}


NetworkService::~NetworkService()
{
}

bool NetworkService::InitializeWSA()
{
	WSADATA w;
	if (WSAStartup(0x0202, &w) != 0)
	{
		sysLog.Log("WSAStartup failed");
		return false;
	}
		
	if (w.wVersion != 0x0202)
	{
		sysLog.Log("Wrong WinSock version");
		return false;
	}

	if (!CreateSocket())
		return false;

	if (!SetupSocketAddress())
		return false;
		
	return true;
}

void NetworkService::ShutdownWSA()
{
	closesocket(sock);
	WSACleanup();
}

bool NetworkService::CreateSocket()
{
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == SOCKET_ERROR || sock == INVALID_SOCKET)
	{
		sysLog.Log("Error creating socket!");
		return false;
	}
	return true;
}

bool NetworkService::SetupSocketAddress(int portNumber)
{
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_addr.s_addr = INADDR_ANY; // UDP sockets should ideally be connectionless, so no need to force it to any ip.
	socketAddress.sin_port = htons(portNumber);

	if(bind(sock, (const sockaddr *)&socketAddress, sizeof(socketAddress)) != 0)
	{
		sysLog.Log("Server Socket bind failed.");
		return false;
	}
	else
	{
		sysLog.Log("Server Socket bound to port.");
	}

	return true;
}

bool NetworkService::WantToRead()
{
	return true;
}

bool NetworkService::WantToWrite()
{
	return true;
}
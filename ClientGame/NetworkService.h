#pragma once
#include <winsock2.h>
#include <string>

#include "Protocol.h"

#pragma comment(lib, "ws2_32.lib")

class NetworkService
{
public:
	NetworkService();
	~NetworkService();

	bool InitializeWSA();
	void ShutdownWSA();

	bool CreateSocket();
	bool SetupSocketAddress(int portNumber = 4444);

	bool WantToRead();
	bool WantToWrite();

	void Send();
	void Receive();

	SOCKET getSocket(){ return sock; }

private:
	SOCKET sock;
	sockaddr_in socketAddress;
};


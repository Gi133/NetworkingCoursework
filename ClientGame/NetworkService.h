#pragma once
#include <winsock2.h>
#include <string>

#include "Protocol.h"
#include "Player.h"

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

	bool Send(MessageType messageType, Player* objectToSend = NULL);
	bool Receive();

	NetworkMessage getReceivedMessage(){ return receivedMessage; }

	SOCKET getSocket(){ return sock; }

private:
	SOCKET sock;
	sockaddr_in socketAddress;

	NetworkMessage* PackMessage(NetworkMessage* networkMessage, MessageType messageType, Player* objectToSend);
	void UnpackMessage(NetworkMessage* networkMessage);

	// Buffer for incoming messages.
	char recv_buf_[sizeof NetworkMessage];
	int recv_count_;
	NetworkMessage receivedMessage;

	// Buffer for outgoing messages.
	char send_buf_[100 * sizeof NetworkMessage];
	int send_count_;
};


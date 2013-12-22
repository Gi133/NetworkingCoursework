#pragma once
#include <winsock2.h>
#include <string>

#include "Protocol.h"
#include "Player.h"

#pragma comment(lib, "ws2_32.lib")

class NetworkService
{
public:
	NetworkService(bool client);
	~NetworkService();

	bool InitializeWSA(bool client);
	void ShutdownWSA();

	bool CreateSocket();
	bool SetupSocketAddressClient();
	bool SetupSocketAddressSpectator();

	bool WantToRead(); // Never required in UDP since it's fire and forget. Added anyway in case ever want to add TCP support for chat.
	bool WantToWrite();

	bool Send(MessageType messageType, Player* objectToSend = NULL);
	bool Receive();

	NetworkMessage getReceivedMessage(){ return receivedMessage; }

	bool CheckMessegeRelevant(); // Check if the net message received is newer than the last one.
	
	SOCKET getSocket(){ return sock; }

private:
	SOCKET sock;
	sockaddr_in socketAddress, fromAddress, toAddress;

	NetworkMessage* PackMessage(NetworkMessage* networkMessage, MessageType messageType, Player* objectToSend);
	void UnpackMessage(NetworkMessage* networkMessage);

	// Buffer for incoming messages.
	char recv_buf_[sizeof NetworkMessage];
	int recv_count_;
	NetworkMessage receivedMessage;
	int fromAddressSize;

	// Buffer for outgoing messages.
	char send_buf_[100 * sizeof NetworkMessage];
	int send_count_;

	// TimeStamp checking variables.
	float oldTimeStamp;
};


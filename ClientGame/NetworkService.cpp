#include "stdafx.h"
#include "NetworkService.h"


NetworkService::NetworkService()
{
	InitializeWSA();

	send_count_ = recv_count_ = 0;
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
		return true;
	}
}

bool NetworkService::WantToRead()
{
	struct pollfd fd;
	int result;

	fd.fd = sock;
	fd.events = POLLIN;

	result = WSAPoll(&fd, 1, 25);

	if (result == 0)
	{
		sysLog.Log("Receive timed out.");
		return false;
	}
	else if (result == -1)
	{
		sysLog.Log("Receive socket error.");
		return false;
	}
	else
	{
		sysLog.Log("Data witting to be received.");
		return true;
	}
}

bool NetworkService::WantToWrite()
{
	return send_count_ > 0;
}

bool NetworkService::Send(MessageType messageType, Player* objectToSend)
{
	NetworkMessage networkMessage;

	if (send_count_ + sizeof(NetworkMessage) > sizeof(send_buf_))
		sysLog.Log("SEND BUFFER IS FULL!");

	memcpy(&send_buf_[send_count_], PackMessage(&networkMessage, messageType, objectToSend), sizeof(NetworkMessage));
	send_count_ += sizeof(NetworkMessage);

	sockaddr_in toAddress;
	toAddress.sin_family = AF_INET;
	toAddress.sin_port = htons(4444);
	toAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

	int count = sendto(sock, send_buf_, send_count_, 0, (const sockaddr *)&toAddress, sizeof(toAddress));
	if (count <= 0)
	{
		sysLog.Log("Client connection closed or broken\n");
		return false;
	}

	send_count_ -= count;
	printf("Sent %d bytes to the client (%d left)\n", count, send_count_);

	// Remove the sent data from the start of the buffer.
	memmove(send_buf_, &send_buf_[count], send_count_);

	return true;
}

bool NetworkService::Receive()
{
	sockaddr_in fromAddress;
	fromAddress.sin_family = AF_INET;
	fromAddress.sin_port = htons(4444);
	fromAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	int fromAddressSize = sizeof(fromAddress);

	// Receive as much data from the client as will fit in the buffer.
	int count = recvfrom(sock, &recv_buf_[recv_count_], (sizeof recv_buf_) - recv_count_, 0, (sockaddr*)&fromAddress, &fromAddressSize);
	if (count <= 0)
	{
		sysLog.Log("Client connection closed or broken.");
		return false;
	}

	printf("Received %d bytes from the client (total %d)\n", count, recv_count_);
	recv_count_ += count;

	// Have we received a complete message?
	if (recv_count_ == sizeof NetworkMessage)
	{
		UnpackMessage((NetworkMessage *)recv_buf_);
		recv_count_ = 0;
	}

	return true;
}

NetworkMessage* NetworkService::PackMessage(NetworkMessage* _networkMessage, MessageType _messageType, Player* _objectToSend)
{
	_networkMessage->messageType = _messageType;
	_networkMessage->timeStamp = theWorld.GetCurrentTimeSeconds();

	if (_messageType != NETMESSAGE_HANDSHAKE || _messageType != NETMESSAGE_UNKNOWN) // Object not required if any of those two.
	{
		_networkMessage->objectID = _objectToSend->GetName();
		_networkMessage->position = _objectToSend->GetPosition();
		_networkMessage->force = _objectToSend->getForce();
	}

	return _networkMessage;
}

void NetworkService::UnpackMessage(NetworkMessage* networkMessage)
{
	networkMessage = &receivedMessage;
}
#include "stdafx.h"
#include "NetworkService.h"


NetworkService::NetworkService(bool client)
{
	oldTimeStamp = 0;

	sock = INVALID_SOCKET;

	send_count_ = recv_count_ = 0;

	InitializeWSA(client);
}


NetworkService::~NetworkService()
{
}

bool NetworkService::InitializeWSA(bool client)
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

	if (client)
	{
		// Set up client socket.
		if(!SetupSocketAddressClient())
			return false;
	}
	else
	{
		// Set up spectator socket.
		if(!SetupSocketAddressSpectator())
			return false;
	}
		
	return true;
}

void NetworkService::ShutdownWSA()
{
	if (sock != INVALID_SOCKET)
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

bool NetworkService::SetupSocketAddressClient()
{
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_addr.s_addr = INADDR_ANY; // UDP sockets should ideally be connectionless, so no need to force it to any ip.
	socketAddress.sin_port = htons(4444);

	toAddress.sin_family = AF_INET;
	toAddress.sin_port = htons(4445);
	toAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

	fromAddress.sin_family = AF_INET;
	fromAddress.sin_port = htons(4445);
	fromAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	fromAddressSize = sizeof(fromAddress);

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

bool NetworkService::SetupSocketAddressSpectator()
{
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_addr.s_addr = INADDR_ANY; // UDP sockets should ideally be connectionless, so no need to force it to any ip.
	socketAddress.sin_port = htons(4445);

	toAddress.sin_family = AF_INET;
	toAddress.sin_port = htons(4444);
	toAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

	fromAddress.sin_family = AF_INET;
	fromAddress.sin_port = htons(4444);
	fromAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	fromAddressSize = sizeof(fromAddress);

	if (bind(sock, (const sockaddr *)&socketAddress, sizeof(socketAddress)) != 0)
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

	result = WSAPoll(&fd, 1, 0);

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
		sysLog.Log("Data waiting to be received.");
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
	if (!WantToRead())
		return false;

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

	if (_messageType == NETMESSAGE_UPDATE) // Object not required if any of those two.
	{
		_networkMessage->ID = _objectToSend->GetName();
		_networkMessage->position = _objectToSend->GetPosition();
		_networkMessage->velocity.X = _objectToSend->GetBody()->GetLinearVelocity().x;
		_networkMessage->velocity.Y = _objectToSend->GetBody()->GetLinearVelocity().y;
	}

	return _networkMessage;
}

void NetworkService::UnpackMessage(NetworkMessage* networkMessage)
{
	receivedMessage = *networkMessage;
}

bool NetworkService::CheckMessegeRelevant()
{
	if (oldTimeStamp != 0) // We have actually had an old timestamp to check against.
	{
		if (oldTimeStamp >= receivedMessage.timeStamp)
		{
			// Message not relevant.
			return false;
		}
		else
		{
			// Message is relevant. Update the oldTimeStamp value.
			oldTimeStamp = receivedMessage.timeStamp;
			return true;
		}
	}
	else
		oldTimeStamp = receivedMessage.timeStamp;
	return true;
}
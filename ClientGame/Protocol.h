// The networking protocol to be used by the game.

#pragma once
#include <string>

// Message Types:
enum MessageType
{
	NETMESSAGE_UNKNOWN = 0,
	NETMESSAGE_CREATE_PLAYER = 1,
	NETMESSAGE_CREATE_BOT = 2,
	NETMESSAGE_UPDATE = 3,
	NETMESSAGE_DELETE = 4
};

// Message struct.
struct  NetworkMessage
{
	MessageType messageType; // Type of message.

	std::string objectID; // ID/Name of the object this message is referring to.
	float positionX, positionY, forceX, forceY; // Object's values to be passed along.

	NetworkMessage() :messageType(NETMESSAGE_UNKNOWN), objectID(""), positionX(0), positionY(0), forceX(0), forceY(0) {}
};
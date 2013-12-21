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
	NETMESSAGE_DELETE = 4,
	NETMESSAGE_HANDSHAKE = 5
};

// Message struct.
struct  NetworkMessage
{
	MessageType messageType; // Type of message.

	float timeStamp;

	std::string objectID; // ID/Name of the object this message is referring to.
	Vector2 position, force;

	NetworkMessage() :messageType(NETMESSAGE_UNKNOWN), timeStamp(0.0f), objectID(""), position(0.0f, 0.0f), force(0.0f, 0.0f) {}
};
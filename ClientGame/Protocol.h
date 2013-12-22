// The networking protocol to be used by the game.

#pragma once
#include <string>
#include "Player.h"

// Message Types:
enum MessageType
{
	NETMESSAGE_UNKNOWN = 0,
	NETMESSAGE_UPDATE = 1,
	NETMESSAGE_DELETE = 2,
	NETMESSAGE_HANDSHAKE = 3
};

// Message struct.
struct  NetworkMessage
{
	MessageType messageType; // Type of message.

	float timeStamp;

	std::string ID;
	Vector2 position;
	Vector2 velocity;

	NetworkMessage() :messageType(NETMESSAGE_UNKNOWN), timeStamp(0.0f), ID(""), position(0.0f, 0.0f), velocity(0.0f, 0.0f) {}
};
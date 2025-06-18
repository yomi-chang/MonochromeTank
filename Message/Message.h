#pragma once
class Message
{
public:
	// メッセージID
	enum MessageID : int
	{
		NONE,
		PATROL,
		TRACKING,
		ATTACK,
		RETREAT_ATTACK,
		AVOIDWALL
	};
};
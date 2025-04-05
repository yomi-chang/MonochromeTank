#pragma once
class Message
{
public:
	// メッセージID
	enum MessageID : int
	{
		PATROL,
		TRACKING,
		ATTACK
	};
};
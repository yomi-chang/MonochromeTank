#pragma once
#include "Message/Message.h"

// IObserverインターフェースを定義する
class IObserver
{
public:
	// メッセージを受け取る
	virtual void OnMessegeAccepted(Message::MessageID messageID) = 0;
};
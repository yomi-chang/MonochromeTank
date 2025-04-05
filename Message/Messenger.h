#pragma once
#include "Message/Message.h"
#include "Interface/IObject.h"
#include <unordered_map>

class Messenger
{
public:
	// Messengerクラスのインスタンスを取得する
	static Messenger* GetInstance();
	// Messengerクラスのインスタンスを破棄する
	static void DestroyInstance();
	// オブジェクトを登録する
	void Register(int objectID, IObject* object);
	// メッセージを送信する
	void Dispatch(int objectID, Message::MessageID);

private:
	// コピー・ムーブ禁止
	Messenger(const Messenger&) = delete;
	Messenger& operator=(const Messenger&) = delete;
	Messenger(Messenger&&) = delete;
	Messenger& operator=(Messenger&&) = delete;
	// コンストラクタ
	Messenger();

private:
	// メッセンジャーインスタンス
	static std::unique_ptr<Messenger> s_messenger;
	// オブジェクトIDとオブジェクトマッピング
	std::unordered_map<int, IObject*> m_objects;
};


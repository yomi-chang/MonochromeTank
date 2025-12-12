#include "pch.h"
#include "Message/Messenger.h"
#include "Interface/ITank.h"


// s_messengerを初期化する
std::unique_ptr<Messenger> Messenger::s_messenger = nullptr;

// コンストラクタ
Messenger::Messenger()
{
}

// Messengerクラスのインスタンスを取得する
Messenger* Messenger::GetInstance()
{
	// Messengerクラスのインスタンス生成されていない場合
	if (s_messenger == nullptr)
	{
		// Messengerクラスのインスタンスを生成する
		s_messenger = std::unique_ptr<Messenger>(new Messenger());
	}
	// Messengerクラスのインスタンスを返す
	return s_messenger.get();
}

/// <summary>
/// インスタンスの破棄
/// </summary>
void Messenger::DestroyInstance()
{
	// Messengerクラスのインスタンスをリセットする
	s_messenger.reset();
}

/// <summary>
/// オブジェクトの登録
/// </summary>
/// <param name="objectID">オブジェクトID</param>
/// <param name="object">オブジェクト</param>
void Messenger::Register(int objectID, ITank* object)
{
	// オブジェクトIDとオブジェクトを登録する
	m_objects.emplace(objectID, object);
}

/// <summary>
/// オブジェクトにメッセージを送信
/// </summary>
/// <param name="objectID">オブジェクトID</param>
/// <param name="messageID">メッセージID</param>
void Messenger::Dispatch(int objectID, Message::MessageID messageID)
{
	// メッセージを送信するオブジェクトを検索する
	auto it = m_objects.find(objectID);
	// メッセージを送信するオブジェクトが見つかった場合
	if (it != m_objects.end())
	{
		// 送信するオブジェクトあのメッセージハンドラを呼び出す
		it->second->OnMessegeAccepted(messageID);
	}
}

/*
	@file	InputManager.cpp
	@brief	入力関係クラス
*/
#include "pch.h"
#include "InputManager.h"

std::unique_ptr<InputManager> InputManager::m_inputManager = nullptr;

// グラフィックスのインスタンスを取得する
InputManager* const InputManager::GetInstance()
{
	if (m_inputManager == nullptr)
	{
		// グラフィックスのインスタンスを生成する
		m_inputManager.reset(new InputManager());
	}
	// グラフィックスのインスタンスを返す
	return m_inputManager.get();
}

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
InputManager::InputManager()
	:
	m_mouseState{},
	m_keyboardState{}
{
}

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
void InputManager::Initialize(const HWND& window)
{
	// マウスを使用できる状態にする
	m_mouse = std::make_unique<DirectX::Mouse>();
	m_mouse->SetWindow(window);
	m_mouseTracker = std::make_unique<DirectX::Mouse::ButtonStateTracker>();

	// キーボードを使用できる状態にする
	m_keyboard = std::make_unique<DirectX::Keyboard>();
	m_keyboardTracker = std::make_unique<DirectX::Keyboard::KeyboardStateTracker>();
}

//---------------------------------------------------------
// マネージャの持つリソースを更新する
//---------------------------------------------------------
void InputManager::Update()
{
	// マウスの情報を更新する
	m_mouseState = m_mouse->GetState();
	m_mouseTracker->Update(m_mouseState);

	// キーボードの情報を更新する
	m_keyboardState = m_keyboard->GetState();
	m_keyboardTracker->Update(m_keyboardState);
}

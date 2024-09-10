/*
	@file	InputManager.h
	@brief	マウスとキーボードの入力を請け負うクラス
*/
#pragma once

class InputManager
{
private:
	// InputManagerクラスのインスタンスへのポインタ
	static std::unique_ptr<InputManager> m_inputManager;

	// マウス
	std::unique_ptr<DirectX::Mouse> m_mouse;
	std::unique_ptr<DirectX::Mouse::ButtonStateTracker> m_mouseTracker;
	DirectX::Mouse::State m_mouseState;

	// キーボード
	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	std::unique_ptr<DirectX::Keyboard::KeyboardStateTracker> m_keyboardTracker;
	DirectX::Keyboard::State m_keyboardState;

	// マウスの移動量
	DirectX::SimpleMath::Vector3 mouseMoveValue;
	// 1フレーム前のマウス座標
	DirectX::SimpleMath::Vector3 previousMousePosition;

public:
	//InputManager(const HWND& window);
	~InputManager() = default;

	// InputManagerクラスのインスタンスを取得する
	static InputManager* const GetInstance();

	void Initialize(const HWND& window);

	void Update();

	// マウスステートを取得する
	const DirectX::Mouse::State& GetMouseState() const
	{
		return m_mouseState;
	}
	// マウスのボタンステートトラッカーを取得する
	const std::unique_ptr<DirectX::Mouse::ButtonStateTracker>& GetMouseTracker() const
	{
		return m_mouseTracker;
	}
	// キーボードステートを取得する
	const DirectX::Keyboard::State& GetKeyboardState() const
	{
		return m_keyboardState;
	}
	// キーボードのステートトラッカーを取得する
	const std::unique_ptr<DirectX::Keyboard::KeyboardStateTracker>& GetKeyboardTracker() const
	{
		return m_keyboardTracker;
	}

	// ToDo マウスの移動量を取得する関数
	DirectX::SimpleMath::Vector3 GetMouseMoveValue();
	// ToDo マウスの移動量を計算する関数

private:
	// コンストラクタ
	InputManager();

	// 代入は許容しない
	void operator=(const InputManager& object) = delete;
	// コピーコンストラクタは許容しない
	InputManager(const InputManager& object) = delete;
};

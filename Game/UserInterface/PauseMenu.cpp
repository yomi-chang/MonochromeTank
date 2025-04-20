/*
	@file	PauseMenu.cpp
	@brief	ポーズ画面クラス
*/
#include "pch.h"
#include "PauseMenu.h"
#include "Framework/Graphics.h"
#include "Framework/Resources.h"
#include "Framework/InputManager.h"
#include "Game/Screen.h"
#include "Libraries/MyLib/Utils.h"

//-------------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------------
PauseMenu::PauseMenu()
	:
	m_graphics{},
	m_resources{},
	m_isDisplay{},
	m_returnTitle{},
	m_currentSelectUi{},
	m_cursorAngle{}
{
	// グラフィックスの設定
	m_graphics = Graphics::GetInstance();

	// リソースの設定
	m_resources = Resources::GetInstance();

	// 初期選択されているUIの設定
	m_currentSelectUi == UI::CANCEL;
}

//-------------------------------------------------------------------
// デストラクタ
//-------------------------------------------------------------------
PauseMenu::~PauseMenu()
{
}

//-------------------------------------------------------------------
// 更新処理
//-------------------------------------------------------------------
void PauseMenu::Update(float elapsedTime)
{
	// キーボードステートの取得
	const auto& kbTracker = InputManager::GetInstance()->GetKeyboardTracker();

	// Escapeキーが押されたらポーズ画面の表示、非表示
	if (kbTracker->IsKeyPressed(DirectX::Keyboard::Q))
	{
		m_isDisplay = m_isDisplay == false ? true : false;
	}

	// 以下ポーズ画面が表示されていないなら早期リターン
	if (!m_isDisplay) { return; }

	// 選択されていない方のUIを選択状態にする
	if (kbTracker->IsKeyPressed(DirectX::Keyboard::W) ||
		kbTracker->IsKeyPressed(DirectX::Keyboard::S))
	{
		m_currentSelectUi = m_currentSelectUi == UI::CANCEL ? UI::TITLE : UI::CANCEL;
	}

	// 選択されていない方のUIを選択状態にする
	if (kbTracker->IsKeyPressed(DirectX::Keyboard::Space))
	{
		switch (m_currentSelectUi)
		{
		case PauseMenu::TITLE:
			this->ReturnToTitle();
			break;
		case PauseMenu::CANCEL:
			this->Cancel();
			break;
		}
	}

	// カーソルの回転
	m_cursorAngle += elapsedTime * CURSOR_SPEED;
}

//-------------------------------------------------------------------
// 描画処理
//-------------------------------------------------------------------
void PauseMenu::Render()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	// 表示しないなら早期リターン
	if (!m_isDisplay) { return; }

	auto spriteBatch = m_graphics->GetSpriteBatch();

	// スプライトバッチの開始
	spriteBatch->Begin();
	Vector4 color = static_cast<DirectX::SimpleMath::Vector4>(DirectX::Colors::Black);
	color.w *= 0.8f;

	// 背景の描画
	RECT screen{ Screen::LEFT,Screen::TOP,Screen::RIGHT,Screen::BOTTOM };
	spriteBatch->Draw(
		m_resources->GetBoxTexture(),
		screen,
		nullptr,
		color
	);

	// テキスト描画
	spriteBatch->Draw(
		m_resources->GetPauseTextTexture(),
		Vector2(Screen::CENTER_X + 100, Screen::CENTER_Y),
		nullptr,
		Colors::White,
		0.0f,
		mylib::GetTextureCenter(m_resources->GetPauseTextTexture()),
		0.7f
	);

	// カーソルの描画
	switch (m_currentSelectUi)
	{
	case PauseMenu::TITLE:
		spriteBatch->Draw(
			m_resources->GetCursorTexture(),
			Vector2(Screen::CENTER_X - 300, Screen::CENTER_Y - 50),
			nullptr,
			Colors::White,
			// カーソルの角度
			m_cursorAngle,
			mylib::GetTextureCenter(m_resources->GetCursorTexture())
		);
		break;
	case PauseMenu::CANCEL:
		spriteBatch->Draw(
			m_resources->GetCursorTexture(),
			Vector2(Screen::CENTER_X - 300, Screen::CENTER_Y + 50),
			nullptr,
			Colors::White,
			m_cursorAngle,
			mylib::GetTextureCenter(m_resources->GetCursorTexture())
		);
		break;
	}

	// スプライトバッチの終了
	spriteBatch->End();
}

//-------------------------------------------------------------------
// ポーズ画面の終了
//-------------------------------------------------------------------
void PauseMenu::Cancel()
{
	m_isDisplay = false;
}

//-------------------------------------------------------------------
// タイトルに戻る
//-------------------------------------------------------------------
void PauseMenu::ReturnToTitle()
{
	m_returnTitle = true;
}

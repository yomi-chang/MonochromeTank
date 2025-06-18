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
#include "Game/Other/SharedData.h"
#include "Libraries/MyLib/SoundManager.h"
#include "Game/UserInterface/Button.h"

/// <summary>
/// コンストラクタ
/// </summary>
PauseMenu::PauseMenu()
	:
	m_graphics{},
	m_resources{},
	m_isDisplay{},
	m_returnTitle{},
	m_currentSelectUi{},
	m_cursorAngle{},
	m_buttons{}
{
	// グラフィックスの設定
	m_graphics = Graphics::GetInstance();

	// リソースの設定
	m_resources = Resources::GetInstance();
}

/// <summary>
/// デストラクタ
/// </summary>
PauseMenu::~PauseMenu()
{
}

/// <summary>
/// 初期化処理
/// </summary>
void PauseMenu::Initialize()
{
	// ボタンの作成
	this->CreateButtons();

	// 初期選択されているUIの設定
	m_currentSelectUi = UI::CANCEL;
}

/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">フレーム間の経過時間</param>
void PauseMenu::Update(float elapsedTime)
{
	// キーボードステートの取得
	const auto& kbTracker = InputManager::GetInstance()->GetKeyboardTracker();

	// Escapeキーが押されたらポーズ画面の表示、非表示
	if (kbTracker->IsKeyPressed(DirectX::Keyboard::Escape))
	{
		m_isDisplay = m_isDisplay == false ? true : false;

		// ポーズ中ならマウスの固定の解除
		if (m_isDisplay)
		{
			InputManager::GetInstance()->UnLockMouseCursor();
		}
		else
		{
			InputManager::GetInstance()->LockMouseCursor();
		}
	}

	// 以下ポーズ画面が表示されていないなら早期リターン
	if (!m_isDisplay) { return; }

	// ボタンがクリックされたときの処理を登録する
	for (auto& button : m_buttons)
	{
		button->CheckOnMouseOver();
		button->CheckClickButton();
	}

	// カーソルの回転
	m_cursorAngle += elapsedTime * CURSOR_SPEED;
}

/// <summary>
/// 描画処理
/// </summary>
void PauseMenu::Render()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	// 表示しないなら早期リターン
	if (!m_isDisplay) { return; }

	auto spriteBatch = m_graphics->GetSpriteBatch();

	// 描画開始
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

	// 描画終了
	spriteBatch->End();

	// ボタンの描画
	for (auto& button : m_buttons)
	{
		button->Render();
	}
}

/// <summary>
/// ボタンの作成
/// </summary>
void PauseMenu::CreateButtons()
{
	using namespace DirectX::SimpleMath;

	// タイトルに戻るボタン
	auto returnTitleButton = std::make_unique<Button>();
	returnTitleButton->Initialize(
		m_resources->GetReturnTitleTextTexture(),
		0.7f,
		Vector2(Screen::CENTER_X + 100, Screen::CENTER_Y - 50)
	);
	// マウス接触処理
	returnTitleButton->SetOnMouseOver([this] {
		m_currentSelectUi = UI::TITLE;
		});
	// マウスクリック処理
	returnTitleButton->SetOnClick([this] {
		this->ReturnToTitle();
		});

	// ポーズ画面の終了ボタン
	auto cancelButton = std::make_unique<Button>();
	cancelButton->Initialize(
		m_resources->GetCancelTextTexture(),
		0.7f,
		Vector2(Screen::CENTER_X + 100, Screen::CENTER_Y + 50)
	);
	// マウス接触処理
	cancelButton->SetOnMouseOver([this] {
		m_currentSelectUi = UI::CANCEL;
		});
	// マウスクリック処理
	cancelButton->SetOnClick([this] {
		this->Cancel();
		// マウスクリックを制限させる
		InputManager::GetInstance()->SetDisableMouseClick(true);
		});

	// ボタンの追加
	m_buttons.emplace_back(std::move(returnTitleButton));
	m_buttons.emplace_back(std::move(cancelButton));
}

/// <summary>
/// ポーズ画面の終了
/// </summary>
void PauseMenu::Cancel()
{
	// ポーズ画面終了
	m_isDisplay = false;

	// マウスの固定
	InputManager::GetInstance()->LockMouseCursor();

	// SEの再生
	SharedData::GetInstance()->GetSoundManager()->PlaySE(XACT_WAVEBANK_SOUNDS_BUTTON_SE);
}

/// <summary>
/// タイトルに戻る
/// </summary>
void PauseMenu::ReturnToTitle()
{
	// タイトルに戻る
	m_returnTitle = true;

	// SEの再生
	SharedData::GetInstance()->GetSoundManager()->PlaySE(XACT_WAVEBANK_SOUNDS_BUTTON_SE);
}

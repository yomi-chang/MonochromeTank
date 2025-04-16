/*
	@file	TitleScene.cpp
	@brief	タイトルシーンクラス
*/
#include "pch.h"
#include "TitleScene.h"
#include "Game/Screen.h"
#include "DeviceResources.h"
#include "Libraries/MyLib/MemoryLeakDetector.h"
#include "Framework/InputManager.h"
#include "Framework/Graphics.h"
#include "Framework/Resources.h"

#include "Game/Objects/Stage/Floor.h"
#include "Game/Objects/Tank/TankBase/Tank.h"
#include "Libraries/MyLib/LockOnCamera.h"
#include "Game/Scene/Fade.h"
#include <cassert>

using namespace DirectX;
using namespace DirectX::SimpleMath;

// ゲーム終了の関数
extern void ExitGame() noexcept;

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
TitleScene::TitleScene()
	:
	m_graphics{ Graphics::GetInstance() },
	m_titleLogo{},
	m_cursorUi{},
	m_isChangeScene{},
	m_floor{},
	m_tanks{},
	m_fade{},
	m_currentSelectUi{},
	m_cursorAngle{}
{
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
TitleScene::~TitleScene()
{
	// do nothing.
}

//---------------------------------------------------------
// 初期化処理
//---------------------------------------------------------
void TitleScene::Initialize()
{	
	// BGMの再生
	SharedData::GetInstance()->GetSoundManager()->PlayBGM(XACT_WAVEBANK_SOUNDS_TITLESCENE_BGM);

	// 画像の受け取り
	m_titleLogo = Resources::GetInstance()->GetTitleLogoTexture();
	m_titleText = Resources::GetInstance()->GetTitleTextTexture();
	m_cursorUi = Resources::GetInstance()->GetCursorTexture();

	// 床の生成
	m_floor = std::make_unique<Floor>(FLOOR_SIZE);
	m_floor->SetTexture(Resources::GetInstance()->GetFloorTexture());

	// 戦車の生成
	m_tanks.push_back(std::make_unique<Tank>(1, Vector3{ -1.5f, 0.1f, -1.5f }, DirectX::XMConvertToRadians(-135.0f)));
	m_tanks.push_back(std::make_unique<Tank>(2, Vector3{ 1.5f, 0.1f, -1.5f }, DirectX::XMConvertToRadians(135.0f)));
	m_tanks.push_back(std::make_unique<Tank>(3, Vector3{ -1.5f, 0.1f, 1.5f }, DirectX::XMConvertToRadians(-45.0f)));
	m_tanks.push_back(std::make_unique<Tank>(4, Vector3{ 1.5f, 0.1f, 1.5f }, DirectX::XMConvertToRadians(45.0f)));
	for (auto& tank : m_tanks)
	{
		tank->Initialize();
	}

	// 射影行列を作成する
	RECT rect{ m_graphics->GetDeviceResources()->GetOutputSize() };
	Matrix projection;
	projection = Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(65.0f),
		static_cast<float>(rect.right) / static_cast<float>(rect.bottom),
		0.1f, 1000.0f
	);

	// 射影行列を設定する
	m_graphics->SetProjectionMatrix(projection);

	// TPSカメラの生成
	m_camera = std::make_unique<mylib::LockOnCamera>();
	m_camera->Initialize();
	m_camera->SetTargetPosition(Vector3::Zero);
	m_camera->SetDistance(CAMERA_DISTANCE);
	m_camera->SetHeight(CAMERA_HEIGHT);
	m_camera->SetEyePosition(CAMERA_EYE_POSITION);

	// シーン遷移用
	m_fade = std::make_unique<Fade>(1.0f);
	m_fade->FadeOut();

	// シーン変更フラグを初期化する
	m_isChangeScene = false;

	// 初期に選択されているUIの設定
	m_currentSelectUi = UI::START;
}

//---------------------------------------------------------
// 更新処理
//---------------------------------------------------------
void TitleScene::Update(float elapsedTime)
{
	// 宣言をしたが、実際は使用していない変数
	UNREFERENCED_PARAMETER(elapsedTime);

	// フェード
	m_fade->Update(elapsedTime);

	// 敵戦車の更新
	for (auto& tank : m_tanks)
	{
		tank->Update(elapsedTime);
	}

	// カメラを更新する
	m_camera->Update(elapsedTime);

	// フェードイン中でフェードが終了しているならシーン遷移
	if (m_fade->GetFadeType() == Fade::FADEIN &&
		m_fade->FinishFade())
	{
		m_isChangeScene = true;
	}
	// フェードが終了していないなら早期リターン
	if (!m_fade->FinishFade()){ return; }

	// キーボードステートの取得
	const auto& kbTracker = InputManager::GetInstance()->GetKeyboardTracker();

	// スペースキーが押された場合
	if (kbTracker->IsKeyPressed(DirectX::Keyboard::Space))
	{
		// 選択されているUIごとの実行
		this->PressSelectUi();
	}

	// 上キーか下キーが押されたらカーソル移動
	if (kbTracker->IsKeyPressed(DirectX::Keyboard::W) ||
		kbTracker->IsKeyPressed(DirectX::Keyboard::S))
	{
		// カーソル移動
		this->MoveCursor();
	}

	m_cursorAngle += elapsedTime * CURSOR_SPEED;
}

//---------------------------------------------------------
// 描画処理
//---------------------------------------------------------
void TitleScene::Render()
{
	auto states = m_graphics->GetCommonStates();
	auto spriteBatch = m_graphics->GetSpriteBatch();

	// ビュー行列の取得
	auto view = Matrix::CreateLookAt(
		m_camera->GetEyePosition(),
		m_camera->GetTargetPosition(),
		Vector3::UnitY
	);
	Graphics::GetInstance()->SetViewMatrix(view);

	// 床の描画
	m_floor->Render();

	// 敵戦車の更新
	for (auto& tank : m_tanks)
	{
		tank->Render();
	}

	// UIの描画
	this->DrawUi();

	// シーン遷移用
	m_fade->Render();
}

//---------------------------------------------------------
// 終了処理
//---------------------------------------------------------
void TitleScene::Finalize()
{
	// do nothing.
}

//---------------------------------------------------------
// 次のシーンIDの取得
//---------------------------------------------------------
IScene::SceneID TitleScene::GetNextSceneID() const
{
	// シーン変更がある場合
	if (m_isChangeScene)
	{
		return IScene::SceneID::SELECT;
	}

	// シーン変更がない場合
	return IScene::SceneID::NONE;
}

//---------------------------------------------------------
// UIの描画
//---------------------------------------------------------
void TitleScene::DrawUi()
{
	auto states = m_graphics->GetCommonStates();
	auto spriteBatch = m_graphics->GetSpriteBatch();

	// スプライトバッチの開始：オプションでソートモード、ブレンドステートを指定する
	spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());

	// ロゴを描画する
	Vector2 pos{ Screen::CENTER_X, Screen::BOTTOM / 3.0f };
	spriteBatch->Draw(
		m_titleLogo,
		pos,
		nullptr,
		Colors::White,
		0.0f,
		mylib::GetTextureCenter(m_titleLogo),
		LOGO_SCALE
	);

	// タイトルテキストの表示
	pos = { Screen::CENTER_X, Screen::CENTER_Y + Screen::CENTER_Y / 2 };
	spriteBatch->Draw(
		m_titleText,
		pos,
		nullptr,
		Colors::White,
		0.0f,
		mylib::GetTextureCenter(m_titleText),
		TITLE_TEXT_SCALE
	);

	// 選択しているUI
	switch (m_currentSelectUi)
	{
	case TitleScene::START:
		spriteBatch->Draw(
			m_cursorUi,
			Vector2(Screen::CENTER_X - 200, Screen::CENTER_Y + 120),
			nullptr,
			Colors::White,
			m_cursorAngle,
			mylib::GetTextureCenter(m_cursorUi),
			CURSOR_SCALE
		);
		break;
	case TitleScene::EXIT:
		spriteBatch->Draw(
			m_cursorUi,
			Vector2(Screen::CENTER_X - 200, Screen::CENTER_Y + Screen::BOTTOM / 3),
			nullptr,
			Colors::White,
			m_cursorAngle,
			mylib::GetTextureCenter(m_cursorUi),
			CURSOR_SCALE
		);
		break;
	default:
		break;
	}

	// スプライトバッチの終わり
	spriteBatch->End();
}

//---------------------------------------------------------
// カーソルの移動
//---------------------------------------------------------
void TitleScene::MoveCursor()
{
	// 選択されていない方のUIを選択する
	m_currentSelectUi = m_currentSelectUi == UI::START ? UI::EXIT : UI::START;
}

//---------------------------------------------------------
// 選択されているUIの決定
//---------------------------------------------------------
void TitleScene::PressSelectUi()
{
	switch (m_currentSelectUi)
	{
	case TitleScene::START:
		// フェード開始
		m_fade->FadeIn();
		break;
	case TitleScene::EXIT:
		// ゲーム終了
		ExitGame();
		break;
	default:
		break;
	}
}

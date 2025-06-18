/*
	@file	ResultScene.cpp
	@brief	リザルトシーンクラス
*/
#include "pch.h"
#include "ResultScene.h"
#include "Game/Screen.h"
#include "DeviceResources.h"
#include "Libraries/MyLib/MemoryLeakDetector.h"
#include "Framework/InputManager.h"
#include "Game/Other/SharedData.h"
#include "Game/Objects/Tank/TankBase/Tank.h"
#include "Libraries/MyLib/LockOnCamera.h"
#include "Game/Objects/Stage/Floor.h"
#include "Game/Scene/Fade.h"
#include "Game/Objects/Stage/StageObject/SkySphere.h"
#include "Game/UserInterface/Button.h"
#include <cassert>

// ゲーム終了の関数
extern void ExitGame() noexcept;

/// <summary>
/// コンストラクタ
/// </summary>
ResultScene::ResultScene()
	:
	m_graphics{Graphics::GetInstance()},
	m_texture{},
	m_isChangeScene{},
	m_tank{},
	m_camera{},
	m_floor{},
	m_texturePos{},
	m_pressSpace{},
	m_buttons{}
{
}

/// <summary>
/// デストラクタ
/// </summary>
ResultScene::~ResultScene()
{
	// do nothing.
}

/// <summary>
/// 初期化処理
/// </summary>
void ResultScene::Initialize()
{
	// テクスチャの受け取り
	m_texture = Resources::GetInstance()->GetResultTexture();
	m_pressSpace = Resources::GetInstance()->GetPressSpaceTexture();

	// シーン変更フラグを初期化する
	m_isChangeScene = false;

	// 勝利した戦車情報を受け取る
	m_tank = SharedData::GetInstance()->GetWinnerTank();
	m_tank->GetCannon()->SetDisplaySight(false);
	if (m_tank->GetTankNumber() == 0)
	{
		// BGMの再生
		SharedData::GetInstance()->GetSoundManager()->PlayBGM(XACT_WAVEBANK_SOUNDS_RESULTSCENEWIN_BGM);
		m_texturePos = { 0,0,700,100 };
	}
	else
	{
		// BGMの再生
		SharedData::GetInstance()->GetSoundManager()->PlayBGM(XACT_WAVEBANK_SOUNDS_RESULTSCENELOSE_BGM);
		m_texturePos = { 0,100,700,200 };
	}

	// TPSカメラの生成
	m_camera = std::make_unique<mylib::LockOnCamera>();
	m_camera->Initialize();
	m_camera->SetTargetPosition(m_tank->GetPosition());
	m_camera->SetEyePosition(DirectX::SimpleMath::Vector3(0.0f, 1.0f, 1.0f));

	// 床の生成
	m_floor = std::make_unique<Floor>(FLOOR_SIZE);
	m_floor->SetTexture(Resources::GetInstance()->GetFloorTexture());

	// シーン遷移用
	m_fade = std::make_unique<Fade>(1.0f);
	m_fade->FadeOut();
	
	// 天球
	m_skySphere = std::make_unique<SkySphere>();

	// マウス固定の解除
	InputManager::GetInstance()->UnLockMouseCursor();

	// ボタンの作成
	this->CreateButton();

	// シーン変更フラグを初期化する
	m_isChangeScene = false;
}

/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">フレーム間の経過時間</param>
void ResultScene::Update(float elapsedTime)
{
	using namespace DirectX::SimpleMath;

	UNREFERENCED_PARAMETER(elapsedTime);

	// フェード
	m_fade->Update(elapsedTime);

	// 戦車の更新
	float angle = DirectX::XMConvertToRadians(0.7f);
	m_tank->GetBody()->Rotate(Quaternion::CreateFromYawPitchRoll(angle, 0.0f, 0.0f));
	m_tank->Update(elapsedTime);

	// フォローカメラを更新する
	m_camera->Update(elapsedTime);

	

	// フェードイン中でフェードが終了しているならシーン遷移
	if (m_fade->GetFadeType() == Fade::FADEIN &&
		m_fade->FinishFade())
	{
		m_isChangeScene = true;
	}

	// フェードが終了していないなら早期リターン
	if (!m_fade->FinishFade()) { return; }

	// ボタンの接触及びクリック処理
	for (auto& button : m_buttons)
	{
		button->CheckOnMouseOver();
		button->CheckClickButton();
	}
}

/// <summary>
/// 描画処理
/// </summary>
void ResultScene::Render()
{
	using namespace DirectX::SimpleMath;

	// ビュー行列の取得
	auto view = Matrix::CreateLookAt(
		m_camera->GetEyePosition(),
		m_camera->GetTargetPosition(),
		Vector3::UnitY
	);
	Graphics::GetInstance()->SetViewMatrix(view);

	// 床の描画
	m_floor->Render();

	// 戦車の描画
	m_tank->Render();

	// 天球の描画
	m_skySphere->Render();

	// UIの描画
	this->DrawUi();

	// シーン遷移用
	m_fade->Render();
}

/// <summary>
/// 終了処理
/// </summary>
void ResultScene::Finalize()
{
	// do nothing.
}

/// <summary>
/// 次のシーンIDの取得
/// </summary>
/// <returns>シーンID</returns>
IScene::SceneID ResultScene::GetNextSceneID() const
{
	// シーン変更がある場合
	if (m_isChangeScene)
	{
		return IScene::SceneID::TITLE;
	}

	// シーン変更がない場合
	return IScene::SceneID::NONE;
}

/// <summary>
/// UIの描画
/// </summary>
void ResultScene::DrawUi()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	auto states = m_graphics->GetCommonStates();
	auto spriteBatch = m_graphics->GetSpriteBatch();

	// ボタンの描画
	for (auto& button : m_buttons)
	{
		button->Render();
	}

	// スプライトバッチの開始：オプションでソートモード、ブレンドステートを指定する
	spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());

	// 勝敗の表示
	spriteBatch->Draw(
		m_texture.Get(),
		Vector2(Screen::CENTER_X, Screen::CENTER_Y),
		&m_texturePos,
		Colors::White,
		0.0f,
		mylib::GetTextureCenter(m_texture.Get()),
		1.0f
	);

	// スプライトバッチの終わり
	spriteBatch->End();
}

/// <summary>
/// ボタンの作成
/// </summary>
void ResultScene::CreateButton()
{
	using namespace DirectX::SimpleMath;

	// タイトルに戻るボタン
	auto returnTitleButton = std::make_unique<Button>();
	returnTitleButton->Initialize(
		Resources::GetInstance()->GetReturnTitleTextTexture(),
		TEXT_SIZE,
		Vector2(Screen::CENTER_X, Screen::CENTER_Y + 220)
	);
	// クリック時の処理
	returnTitleButton->SetOnClick([this]() {
		// フェード開始
		m_fade->FadeIn();
	});

	// ゲーム終了ボタン
	auto exitButton = std::make_unique<Button>();
	exitButton->Initialize(
		Resources::GetInstance()->GetExitTextTexture(),
		TEXT_SIZE,
		Vector2(Screen::CENTER_X, Screen::CENTER_Y + 300)
	);
	// クリック時の処理
	exitButton->SetOnClick([this]() {
		// ゲーム終了
		ExitGame();
	});

	// ボタン情報配列に譲渡する
	m_buttons.emplace_back(std::move(returnTitleButton));
	m_buttons.emplace_back(std::move(exitButton));
}

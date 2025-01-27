/*
	@file	SelectScene.cpp
	@brief	セレクトシーンクラス
*/
#include "pch.h"
#include "SelectScene.h"
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
#include "Game/Other/SharedData.h"
#include <cassert>

using namespace DirectX;
using namespace DirectX::SimpleMath;

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
SelectScene::SelectScene()
	:
	m_graphics{ Graphics::GetInstance() },
	m_spriteBatch{},
	m_isChangeScene{},
	m_floor{},
	m_fade{},
	m_resources{ Resources::GetInstance() },
	m_selectAngle {}
{
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
SelectScene::~SelectScene()
{
	// do nothing.
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void SelectScene::Initialize()
{
	auto device = m_graphics->GetDeviceResources()->GetD3DDevice();
	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
	
	// スプライトバッチを作成する
	m_spriteBatch = m_graphics->GetSpriteBatch();


	// 床の生成
	m_floor = std::make_unique<Floor>(50);
	m_floor->SetTexture(Resources::GetInstance()->GetFloorTexture());

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
	m_camera->SetTargetPosition(Vector3(0, 0, 0));
	m_camera->SetDistance(1.0f);
	m_camera->SetHeight(3.0f);
	m_camera->SetEyePosition(DirectX::SimpleMath::Vector3(0.0f, 5.0f, 5.0f));

	// シーン遷移用
	m_fade = std::make_unique<Fade>(1.0f);
	m_fade->FadeOut();

	// シーン変更フラグの初期化
	m_isChangeScene = false;

	// ゲーム設定の初期設定
	m_selectPos = SELECT_POS1;
	m_stageTexturePos = STAGE1;
	m_tankCountTexturePos = TANK_COUNT3;
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void SelectScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// フェード
	m_fade->Update(elapsedTime);

	// フォローカメラを更新する
	m_camera->Update(elapsedTime);

	// フェードイン中でフェードが終了しているならシーン遷移
	if (m_fade->GetFadeType() == Fade::FADEIN &&
		m_fade->FinishFade())
	{
		// ステージ設定の設定
		SetStageSetting();
		// シーン遷移フラグ
		m_isChangeScene = true;
	}

	// フェードが終了していないなら早期リターン
	if (!m_fade->FinishFade()) { return; }


	// キーボードステートの取得
	const auto& kbTracker = InputManager::GetInstance()->GetKeyboardTracker();

	// カーソル移動
	if (kbTracker->IsKeyPressed(DirectX::Keyboard::S))
	{
		if (m_selectPos == SELECT_POS1) { m_selectPos = SELECT_POS2; }
		else if (m_selectPos == SELECT_POS2) { m_selectPos = SELECT_POS3; }
	}
	if (kbTracker->IsKeyPressed(DirectX::Keyboard::W))
	{
		if (m_selectPos == SELECT_POS3) { m_selectPos = SELECT_POS2; }
		else if (m_selectPos == SELECT_POS2) { m_selectPos = SELECT_POS1; }
	}

	// ステージ選択
	if (m_selectPos == SELECT_POS1)
	{
		if (kbTracker->IsKeyPressed(DirectX::Keyboard::D))
		{
			if (m_stageTexturePos == STAGE1) { m_stageTexturePos = STAGE2; }
			else if (m_stageTexturePos == STAGE2) { m_stageTexturePos = STAGE3; }
			else if (m_stageTexturePos == STAGE3) { m_stageTexturePos = STAGE1; }
		}
		if (kbTracker->IsKeyPressed(DirectX::Keyboard::A))
		{
			if (m_stageTexturePos == STAGE1) { m_stageTexturePos= STAGE3; }
			else if (m_stageTexturePos == STAGE2) { m_stageTexturePos = STAGE1; }
			else if (m_stageTexturePos == STAGE3) { m_stageTexturePos = STAGE2; }
		}
	}

	// 戦車数選択
	if (m_selectPos == SELECT_POS2)
	{
		if (kbTracker->IsKeyPressed(DirectX::Keyboard::D))
		{
			if (m_tankCountTexturePos == TANK_COUNT1) { m_tankCountTexturePos = TANK_COUNT2; }
			else if (m_tankCountTexturePos == TANK_COUNT2) { m_tankCountTexturePos = TANK_COUNT3; }
			else if (m_tankCountTexturePos == TANK_COUNT3) { m_tankCountTexturePos = TANK_COUNT1; }
		}
		if (kbTracker->IsKeyPressed(DirectX::Keyboard::A))
		{
			if (m_tankCountTexturePos == TANK_COUNT1) { m_tankCountTexturePos = TANK_COUNT3; }
			else if (m_tankCountTexturePos == TANK_COUNT2) { m_tankCountTexturePos = TANK_COUNT1; }
			else if (m_tankCountTexturePos == TANK_COUNT3) { m_tankCountTexturePos = TANK_COUNT2; }
		}
	}

	// スペースキーが押されたらフェード開始
	if (kbTracker->IsKeyPressed(DirectX::Keyboard::Space) &&
		m_selectPos == SELECT_POS3)
	{
		m_fade->FadeIn();
	}

	m_selectAngle += elapsedTime * 2.0f;
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void SelectScene::Render()
{
	using namespace DirectX::SimpleMath;

	auto states = m_graphics->GetCommonStates();

	// ビュー行列の取得
	auto view = Matrix::CreateLookAt(
		m_camera->GetEyePosition(),
		m_camera->GetTargetPosition(),
		Vector3::UnitY
	);
	Graphics::GetInstance()->SetViewMatrix(view);

	m_floor->Render();

	// スプライトバッチの開始：オプションでソートモード、ブレンドステートを指定する
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());

	// チュートリアル表示
	m_spriteBatch->Draw(
		m_resources->GetManualTexture(),
		Vector2{ 550,90 },
		nullptr,
		DirectX::Colors::White,
		0.0f,
		Vector2{ 0,0 },
		Vector2{ 0.5f,0.5f }
	);

	// 設定フレーム
	m_spriteBatch->Draw(
		m_resources->GetSettingTexture(),
		Vector2{ 40,70 },
		nullptr,
		DirectX::Colors::White,
		0.0f,
		Vector2{ 0,0 },
		Vector2{ 0.5f,0.5f }
	);

	// ステージ
	m_spriteBatch->Draw(
		m_resources->GetStageTextTexture(),
		Vector2{ 210,280 },
		&m_stageTexturePos,
		DirectX::Colors::White,
		0.0f,
		Vector2{ 50,50 },
		Vector2{ 0.5f,0.5f }
	);

	// 戦車数
	m_spriteBatch->Draw(
		m_resources->GetCountTextTexture(),
		Vector2{ 300,415 },
		&m_tankCountTexturePos,
		DirectX::Colors::White,
		0.0f,
		Vector2{ 50,50 },
		Vector2{ 0.5f,0.5f }
	);

	// カーソル
	m_spriteBatch->Draw(
		m_resources->GetSelectTexture(),
		m_selectPos,
		nullptr,
		DirectX::Colors::White,
		m_selectAngle,
		Vector2{ 50,50 },
		Vector2{ 1.0f,1.0f }
	);

	// スプライトバッチの終わり
	m_spriteBatch->End();

	// シーン遷移用
	m_fade->Render();
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void SelectScene::Finalize()
{
	// do nothing.
}

//---------------------------------------------------------
// 次のシーンIDを取得する
//---------------------------------------------------------
IScene::SceneID SelectScene::GetNextSceneID() const
{
	// シーン変更がある場合
	if (m_isChangeScene)
	{
		return IScene::SceneID::PLAY;
	}

	// シーン変更がない場合
	return IScene::SceneID::NONE;
}

//---------------------------------------------------------
// ステージ設定の設定
//---------------------------------------------------------
void SelectScene::SetStageSetting()
{
	int tankCount;
	std::string stageName;

	// 戦車の数
	if (m_tankCountTexturePos == TANK_COUNT1)	   { tankCount = 1; }
	else if (m_tankCountTexturePos == TANK_COUNT2) { tankCount = 2; }
	else if (m_tankCountTexturePos == TANK_COUNT3) { tankCount = 3; }

	// ステージ名
	if (m_stageTexturePos == STAGE1)	  { stageName = "Stage1"; }
	else if (m_stageTexturePos == STAGE2) { stageName = "Stage2"; }
	else if (m_stageTexturePos == STAGE3) { stageName = "Stage3"; }

	// データの保存
	SharedData::GetInstance()->SetSelectData(tankCount, stageName);
}

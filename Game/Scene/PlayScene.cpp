/*
	@file	PlayScene.cpp
	@brief	プレイシーンクラス
*/
#include "pch.h"
#include "PlayScene.h"
#include "DeviceResources.h"

#include "Framework/Graphics.h"
#include "Framework/InputManager.h"
#include "Framework/Resources.h"

#include "Libraries/MyLib/DebugCamera.h"
#include "Libraries/MyLib/DebugString.h"
#include "Libraries/MyLib/FollowCamera.h"
#include "Libraries/Microsoft/DebugDraw.h"
#include "Libraries/MyLib/MemoryLeakDetector.h"

#include "Game/Objects/Stage/StageObject/SkySphere.h"
#include "Game/Objects/Stage/Wall.h"
#include "Game/Objects/Tank/PlayerTank.h"
#include "Game/Objects/Tank/EnemyTanks/EnemyTank.h"

#include "Game/UserInterface/MagazineUi.h"

#include "Game/Objects/Stage/StageManager.h"
#include "Game/Other/CollisionManager.h"
#include "Game/Scene/Fade.h"
#include "Game/Other/ResultData.h"

#include <cassert>

using namespace DirectX;
using namespace DirectX::SimpleMath;

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
PlayScene::PlayScene()
	:
	m_graphics{Graphics::GetInstance()},
	m_debugCamera{},
	m_tpsCamera{},
	m_cameraType{CameraType::TPS},
	m_isChangeScene{},
	m_player{},
	m_enemies{},
	m_stageManager{},
	m_collisonManager{},
	m_fade{},
	m_isStart{}
{
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
PlayScene::~PlayScene()
{
	// do nothing.
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void PlayScene::Initialize()
{
	using namespace DirectX::SimpleMath;

	// デバッグカメラを作成する
	RECT rect{ m_graphics->GetDeviceResources()->GetOutputSize() };
	m_debugCamera = std::make_unique<mylib::DebugCamera>();
	m_debugCamera->Initialize(rect.right, rect.bottom);

	// 射影行列を作成する
	Matrix projection;
	projection = Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(65.0f),
		static_cast<float>(rect.right) / static_cast<float>(rect.bottom),
		0.1f, 1000.0f
	);
	
	// 射影行列を設定する
	m_graphics->SetProjectionMatrix(projection);

	// シーン変更フラグを初期化する
	m_isChangeScene = false;

	// オブジェクトの生成============================================================
	// プレイヤー戦車の生成
	m_player = std::make_unique<PlayerTank>(0, Vector3::Zero);
	m_player->Initialize();

	// 敵戦車
	m_enemies.push_back(std::make_unique<EnemyTank>(1, Vector3{ 0.0f, 0.0f, -8.0f }));
	m_enemies.push_back(std::make_unique<EnemyTank>(2, Vector3{ -5.0f, 0.0f, -5.0f }));
	m_enemies.push_back(std::make_unique<EnemyTank>(3, Vector3{ 3.0f, 0.0f, 5.0f }));
	for (auto& enemy : m_enemies)
	{
		enemy->Initialize();
	}

	// TPSカメラの生成
	m_tpsCamera = std::make_unique<mylib::FollowCamera>();
	m_tpsCamera->Initialize(m_player->GetTank());

	// ステージマネージャーの生成
	m_stageManager = std::make_unique<StageManager>();
	m_stageManager->Initialize();

	// UI関係
	m_magazine = std::make_unique<MagazineUi>();
	m_magazine->Initialize();
	m_magazine->SetPlayer(m_player.get());

	// コリジョンマネージャー
	m_collisonManager = std::make_unique<CollisionManager>();

	// シーン遷移用
	m_fade = std::make_unique<Fade>(1.0f);
	m_fade->FadeOut();

	//全戦車の情報を持つ配列
	std::vector<Tank*> tankPointers;
	tankPointers.push_back(m_player->GetTank());
	for (auto& tank : m_enemies)
	{
		tankPointers.push_back(tank->GetTank());
	}

	// 必要な情報の設定============================================================
	// 各戦車に全戦車の情報を設定
	m_player->SetOtherTanks(tankPointers);
	for (auto& tank : m_enemies)
	{
		tank->SetOtherTanks(tankPointers);
	}
	// 壁に戦車情報の設定
	m_stageManager->SetObjectData(tankPointers, m_tpsCamera.get());
	// カメラ情報の設定
	m_player->SetCamera(m_tpsCamera.get());
	// 全オブジェクトの情報の設定
	m_collisonManager->SetObjectData(
		tankPointers,
		m_tpsCamera.get(),
		m_stageManager->GetFixedTurret(),
		m_stageManager->GetWalls(),
		m_stageManager->GetWallGimmick()
	);
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void PlayScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// シーン遷移用
	m_fade->Update(elapsedTime);

	// フォローカメラを更新する
	m_tpsCamera->Update(elapsedTime);

	// デバッグカメラを更新する
	m_debugCamera->Update();

	// ゲーム開始
	if (!m_isStart)
	{
		// フェードが終了したらゲーム開始
		if (m_fade->FinishFade())
		{
			m_isStart = true;
		}
		else
		{
			return;
		}
	}

	// コリジョンマネージャーの更新
	m_collisonManager->Update();

	// 戦車の更新
	m_player->Update(elapsedTime);

	// 敵戦車の更新
	for (auto& enemy : m_enemies)
	{
		enemy->Update(elapsedTime);
	}

	// ステージの更新
	m_stageManager->Update(elapsedTime);

	// Cキーを押すことでデバッグカメラとTPSカメラを切り替える
	const auto& keyboardTracker = InputManager::GetInstance()->GetKeyboardTracker();
	if (keyboardTracker->IsKeyPressed(DirectX::Keyboard::C))
	{
		this->ChangeCameraType();
	}

	// 敵を全て倒していたらリザルトシーンへ ToDo:現在1体倒したら終わるので修正する
	// 生存している戦車の確認
	int surviveTank = 0;
	if (!m_player->GetDead())
		surviveTank++;
	for (auto& enemy : m_enemies)
	{
		if (!enemy->GetDead())
			surviveTank++;
	}

	// フェードが終了していたらリザルトシーンに
	if (m_fade->FinishFade())
	{
		//生存している戦車情報をRetultDataに所有権ごと渡す
		if (!m_player->GetDead())
		{
			ResultData::GetInstance()->SetWinnerTank(m_player->ReleaseTank());
		}
		for (auto& enemy : m_enemies)
		{
			if (!enemy->GetDead())
				ResultData::GetInstance()->SetWinnerTank(enemy->ReleaseTank());
		}
		m_isChangeScene = true;
	}

	// 生存している戦車が1台だけならゲーム終了
	if (surviveTank == 1)
	{
		// フェード開始
		m_fade->FadeIn();
	}
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void PlayScene::Render()
{
	// カメラタイプに応じたビュー行列の取得
	auto view = DirectX::SimpleMath::Matrix::Identity;
	switch (m_cameraType)
	{
		case CameraType::TPS:
			view = Matrix::CreateLookAt(
				m_tpsCamera->GetEyePosition(),
				m_tpsCamera->GetTargetPosition(),
				Vector3::UnitY
			);
			Graphics::GetInstance()->SetViewMatrix(view);
			break;
		case CameraType::DEBUG:
			view = m_debugCamera->GetViewMatrix();
			Graphics::GetInstance()->SetViewMatrix(view);
			break;
	}
	Graphics::GetInstance()->SetViewMatrix(view);

	// ステージの描画
	m_stageManager->Render();

	// 敵の描画
	for (auto& enemy : m_enemies)
	{
		enemy->Render();
	}

	// 戦車の描画
	m_player->Render();

	// UI関係
	m_magazine->Render();

	// シーン遷移用
	m_fade->Render();

	// デバッグ情報を「DebugString」で表示する
#ifdef _DEBUG
	/*auto debugString = m_graphics->GetDebugString();
	debugString->AddString("Play Scene");
	debugString->AddString(" ");
	debugString->AddString("PlayerTank");
	debugString->AddString("x : %f", m_playerTank->GetTankPosition().x);
	debugString->AddString("z : %f", m_playerTank->GetTankPosition().z);
	debugString->AddString(" ");
	switch (m_playerTank->GetBulletType())
	{
		case Tank::BULLET:
			debugString->AddString("Bullet");
			debugString->AddString("value : %d", m_playerTank->GetBulletValue());
			break;
		case Tank::CANNONBALL:
			debugString->AddString("CannonBall");
			debugString->AddString("value : %d", m_playerTank->GetCannonBallValue());
		default:
			break;
	}*/
#endif
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void PlayScene::Finalize()
{
	// do nothing.
}

//---------------------------------------------------------
// 次のシーンIDを取得する
//---------------------------------------------------------
IScene::SceneID PlayScene::GetNextSceneID() const
{
	// シーン変更がある場合
	if (m_isChangeScene)
	{
		return IScene::SceneID::RESULT;
	}

	// シーン変更がない場合
	return IScene::SceneID::NONE;
}

//---------------------------------------------------------
// カメラタイプを変更する
//---------------------------------------------------------
void PlayScene::ChangeCameraType()
{
	// 選択されていない方のカメラタイプにする
	switch (m_cameraType)
	{
		case PlayScene::TPS:
			m_cameraType = CameraType::DEBUG;
			// マウスカーソルの解放
			InputManager::GetInstance()->UnLockMouseCursor();
			break;
		case PlayScene::DEBUG:
			m_cameraType = CameraType::TPS;
			// マウスカーソルの固定
			InputManager::GetInstance()->LockMouseCursor();
			break;
		default:
			break;
	}
}

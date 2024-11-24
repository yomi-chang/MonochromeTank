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

#include "Interface/IComponent.h"

#include "Game/Objects/Other/SkySphere.h"
#include "Game/Objects/Stage/Wall.h"
#include "Game/Objects/NewTank/PlayerTank.h"
#include "Game/Objects/NewTank/EnemyTanks/SimpleTank.h"

#include "Game/UserInterface/Magazine.h"

#include "Game/Objects/Stage/StageManager.h"

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
	m_stageManager{}
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

	auto device = m_graphics->GetDeviceResources()->GetD3DDevice();
	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
	//auto states = m_graphics->GetCommonStates();

	// デバッグカメラを作成する
	RECT rect{ m_graphics->GetDeviceResources()->GetOutputSize() };
	m_debugCamera = std::make_unique<mylib::DebugCamera>();
	m_debugCamera->Initialize(rect.right, rect.bottom);

	// 射影行列を作成する
	Matrix projection;
	projection = Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(45.0f),
		static_cast<float>(rect.right) / static_cast<float>(rect.bottom),
		0.1f, 1000.0f
	);
	
	// 射影行列を設定する
	m_graphics->SetProjectionMatrix(projection);

	// モデルの読み込み(GameClassの方がいいかも)
	Resources::GetInstance()->LoadResource();

	// シーン変更フラグを初期化する
	m_isChangeScene = false;

	// 戦車
	//m_playerTank = std::make_unique<Tank>(nullptr, Vector3(0.0f, 0.0f,5.0f), 0.0f);
	//m_playerTank->Initialize(IComponent::Type::PLAYER);

	// プレイヤー戦車の生成
	m_player = std::make_unique<PlayerTank>();
	m_player->Initialize();

	// 敵戦車
	m_enemies.push_back(std::make_unique<EnemyTank>(Vector3{ -5.0f, 0.0f, -10.0f }));
	m_enemies.push_back(std::make_unique<EnemyTank>(Vector3{ 5.0f, 0.0f, -10.0f }));
	std::vector<EnemyTank*> enemyPointers;
	for (auto& enemy : m_enemies)
	{
		enemy->Initialize();
		enemy->SetPlayerTank(m_player.get());

		enemyPointers.push_back(enemy.get());
	}

	// TPSカメラの生成
	m_tpsCamera = std::make_unique<mylib::FollowCamera>();
	m_tpsCamera->Initialize(m_player.get());

	// ステージマネージャーの生成
	m_stageManager = std::make_unique<StageManager>();
	m_stageManager->Initialize();
	m_stageManager->SetObjectData(m_player.get(), m_tpsCamera.get(), enemyPointers);

	// UI関係
	m_magazine = std::make_unique<Magazine>();
	m_magazine->Initialize();

	// カメラ情報を渡す
	m_player->SetCamera(m_tpsCamera.get());
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void PlayScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// 戦車の更新
	m_player->Update(elapsedTime);

	// 敵戦車の更新
	for (auto& enemy : m_enemies)
	{
		enemy->Update(elapsedTime);
	}

	// フォローカメラを更新する
	m_tpsCamera->Update(elapsedTime);

	// デバッグカメラを更新する
	m_debugCamera->Update();

	// Cキーを押すことでデバッグカメラとTPSカメラを切り替える
	const auto& keyboardTracker = InputManager::GetInstance()->GetKeyboardTracker();
	if (keyboardTracker->IsKeyPressed(DirectX::Keyboard::C))
	{
		this->ChangeCameraType();
	}

	// 敵を全て倒していたらリザルトシーンへ ToDo:現在1体倒したら終わるので修正する
	for (auto& enemy : m_enemies)
	{
		if (enemy->GetDead())
		{
			m_isChangeScene = true;
		}
	}
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void PlayScene::Render()
{
	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_graphics->GetCommonStates();
	
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

	// 戦車の描画
	m_player->Render();

	// 敵の描画
	for (auto& enemy : m_enemies)
	{
		enemy->Render();
	}

	// UI関係
	m_magazine->Render();

	SpriteBatch* spriteBatch = m_graphics->GetSpriteBatch();
	ID3D11ShaderResourceView* texture = Resources::GetInstance()->GetCannonBallTexture();
	RECT rect = { 1100,600,1200,650 };
	spriteBatch->Begin();
	if (m_player->GetTankCannon()->GetCannonBall()->GetBulletState() == IBullet::UNUSED)
	{
		spriteBatch->Draw(texture, rect);
	}
	else
	{
		spriteBatch->Draw(texture, rect, DirectX::Colors::Gray);
	}
	spriteBatch->End();

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

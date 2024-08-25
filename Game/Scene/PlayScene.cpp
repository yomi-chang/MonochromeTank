/*
	@file	PlayScene.cpp
	@brief	プレイシーンクラス
*/
#include "pch.h"
#include "PlayScene.h"
#include "DeviceResources.h"
#include "Libraries/MyLib/DebugCamera.h"
#include "Libraries/MyLib/DebugString.h"
#include "Libraries/Microsoft/DebugDraw.h"
#include "Libraries/MyLib/GridFloor.h"
#include "Libraries/MyLib/MemoryLeakDetector.h"
#include "Framework/Resources.h"
#include "Libraries/MyLib/FollowCamera.h"
#include "Libraries/MyLib/CollisionMesh.h"
#include "Interface/IComponent.h"
#include "Framework/InputManager.h"

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
	m_gridFloor{},
	m_projection{},
	m_isChangeScene{},
	m_skySphere{},
	m_playerTank{},
	m_collisionMesh{},
	m_enemyTanks{},
	m_walls{}
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
	auto device = m_graphics->GetDeviceResources()->GetD3DDevice();
	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_graphics->GetCommonStates();
	
	// グリッド床を作成する
	m_gridFloor = std::make_unique<mylib::GridFloor>(device, context, states);

	// デバッグカメラを作成する
	RECT rect{ m_graphics->GetDeviceResources()->GetOutputSize() };
	m_debugCamera = std::make_unique<mylib::DebugCamera>();
	m_debugCamera->Initialize(rect.right, rect.bottom);

	// 射影行列を作成する
	m_projection = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(45.0f),
		static_cast<float>(rect.right) / static_cast<float>(rect.bottom),
		0.1f, 1000.0f
	);
	
	// 射影行列を設定する
	m_graphics->SetProjectionMatrix(m_projection);

	// モデルの読み込み(GameClassの方がいいかも)
	Resources::GetInstance()->LoadResource();

	// シーン変更フラグを初期化する
	m_isChangeScene = false;

	// 天球
	m_skySphere = std::make_unique<SkySphere>();

	// 戦車
	m_playerTank = std::make_unique<Tank>(nullptr, Vector3(0.0f, 0.0f,5.0f), 0.0f);
	m_playerTank->Initialize(IComponent::Type::PLAYER);

	//敵戦車
	m_enemyTanks.emplace_back(std::make_unique<Tank>(nullptr, Vector3(0.0f, 0.0f, -10.0f), DirectX::XMConvertToRadians(180.0f)));
	//m_enemyTanks.emplace_back(std::make_unique<Tank>(nullptr, Vector3(10.0f, 0.0f, -10.0f), DirectX::XMConvertToRadians(180.0f)));
	for (auto& enemyTank : m_enemyTanks)
	{
		// 敵にプレイヤー戦車情報を渡す
		enemyTank->Initialize(IComponent::Type::ENEMY);
		enemyTank->SetOtherTank(m_playerTank.get());

		// プレイヤーに敵戦車情報を渡す
		m_playerTank->SetOtherTank(enemyTank.get());
	}

	// TPSカメラの生成
	m_tpsCamera = std::make_unique<mylib::FollowCamera>();
	m_tpsCamera->Initialize(m_playerTank.get());

	// コリジョンメッシュを生成する
	m_collisionMesh = std::make_unique<mylib::CollisionMesh>();
	m_collisionMesh->Initialize(device, context, L"Terrain");

	// ステージの生成
	m_walls.emplace_back(std::make_unique<Wall>(Vector3(40.0f, 5.0f, 0.5f), Vector3(0.0f,2.5f,-20.0f)));
	m_walls.emplace_back(std::make_unique<Wall>(Vector3(40.0f, 5.0f, 0.5f), Vector3(0.0f, 2.5f, 20.0f)));
	m_walls.emplace_back(std::make_unique<Wall>(Vector3(0.5f, 5.0f, 40.0f), Vector3(20.0f, 2.5f, 0.0f)));
	m_walls.emplace_back(std::make_unique<Wall>(Vector3(0.5f, 5.0f, 40.0f), Vector3(-20.0f, 2.5f, 0.0f)));
	for (auto& wall : m_walls)
	{
		wall->SetPlayer(m_playerTank.get());
	}
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void PlayScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// ビュー行列を取得する
	//const Matrix& view = m_debugCamera->GetViewMatrix();

	// 戦車の更新処理
	Vector3 position(0.0f, 0.0f, 0.0f);
	float angle = 0.0f;
	m_playerTank->Update(elapsedTime,position,angle);

	for (auto& enemyTank : m_enemyTanks)
	{
		enemyTank->Update(elapsedTime, position, angle);

		// プレイヤーか敵の体力のどちらかの体力が０ならリザルトへ
		if (enemyTank->GetDead() ||
			m_playerTank->GetDead())
		{
			m_isChangeScene = true;
		}
	}

	// フォローカメラを更新する
	m_tpsCamera->Update(elapsedTime);

	// デバッグカメラを更新する
	m_debugCamera->Update();

	// Cキーを押すことでデバッグカメラとTPSカメラを切り替える
	const auto& keyboardTracker = InputManager::GetInstance()->GetKeyboardTracker();
	if (keyboardTracker->IsKeyPressed(DirectX::Keyboard::C))
	{
		// 選択されていない方のカメラタイプにする
		m_cameraType = (m_cameraType == CameraType::TPS) ? CameraType::DEBUG : CameraType::TPS;
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

	// 格子床を描画する
	//m_gridFloor->Render(context, Graphics::GetInstance()->GetViewMatrix(), m_projection);

	// 天球の描画
	m_skySphere->Render();

	// メッシュを描画する
	m_collisionMesh->Draw(
		context, states,
		Graphics::GetInstance()->GetViewMatrix(),
		Graphics::GetInstance()->GetProjectionMatrix()
	);

	// ステージの描画
	for (auto& wall : m_walls)
	{
		wall->Render();
	}

	//戦車の描画
	for (auto& enemyTank : m_enemyTanks)
	{
		enemyTank->Render();
	}
	m_playerTank->Render();

	// キーボードステートの取得
	DirectX::Mouse::State mouseState = DirectX::Mouse::Get().GetState();
	float mousePosX = 1.0f / static_cast<float>(mouseState.x);
	float mousePosY = 1.0f / static_cast<float>(mouseState.y);

	// デバッグ情報を「DebugString」で表示する
#ifdef _DEBUG
	auto debugString = m_graphics->GetDebugString();
	debugString->AddString("Play Scene");
	debugString->AddString(" ");
	debugString->AddString("PlayerTank");
	debugString->AddString("x : %f", m_playerTank->GetTankPosition().x);
	debugString->AddString("z : %f", m_playerTank->GetTankPosition().z);
	debugString->AddString("angle : %f", DirectX::XMConvertToDegrees(m_playerTank->GetTankAngleRL()));
	debugString->AddString(" ");
	debugString->AddString("MousePosition");
	debugString->AddString("x : %d", mouseState.x);
	debugString->AddString("y : %d", mouseState.y);
	debugString->AddString("x : %f", mousePosX);
	debugString->AddString("y : %f", mousePosY);
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
	}
	debugString->AddString(" ");
	debugString->AddString("time : %f", m_playerTank->GetTime());
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
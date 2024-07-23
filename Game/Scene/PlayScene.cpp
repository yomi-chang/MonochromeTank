/*
	@file	PlayScene.cpp
	@brief	プレイシーンクラス
*/
#include "pch.h"
#include "PlayScene.h"
#include "Game/CommonResources.h"
#include "DeviceResources.h"
#include "Libraries/MyLib/DebugCamera.h"
#include "Libraries/MyLib/DebugString.h"
#include "Libraries/Microsoft/DebugDraw.h"
#include "Libraries/MyLib/GridFloor.h"
#include "Libraries/MyLib/InputManager.h"
#include "Libraries/MyLib/MemoryLeakDetector.h"

#include "Framework/Resources.h"
#include "Libraries/MyLib/FollowCamera.h"
#include "Libraries/MyLib/CollisionMesh.h"
#include "Interface/IComponent.h"

#include <cassert>

using namespace DirectX;
using namespace DirectX::SimpleMath;

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
PlayScene::PlayScene()
	:
	m_commonResources{},
	m_debugCamera{},
	m_gridFloor{},
	m_projection{},
	m_isChangeScene{},
	m_skySphere{},
	m_playerTank{},
	m_tpsCamera{},
	m_collisionMesh{},
	m_enemyTank{}
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
void PlayScene::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();


	// グリッド床を作成する
	m_gridFloor = std::make_unique<mylib::GridFloor>(device, context, states);

	// デバッグカメラを作成する
	RECT rect{ m_commonResources->GetDeviceResources()->GetOutputSize() };
	m_debugCamera = std::make_unique<mylib::DebugCamera>();
	m_debugCamera->Initialize(rect.right, rect.bottom);

	// 射影行列を作成する
	m_projection = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(45.0f),
		static_cast<float>(rect.right) / static_cast<float>(rect.bottom),
		0.1f, 1000.0f
	);
	
	// 射影行列を設定する
	Graphics::GetInstance()->SetProjectionMatrix(m_projection);

	// モデルの読み込み(GameClassの方がいいかも)
	Resources::GetInstance()->LoadResource();

	// シーン変更フラグを初期化する
	m_isChangeScene = false;

	// 天球
	m_skySphere = std::make_unique<SkySphere>();

	// 戦車
	m_playerTank = std::make_unique<Tank>(nullptr, Vector3(0.0f, 0.0f, 10.0f), 0.0f);
	m_playerTank->Initialize(IComponent::Type::PLAYER);

	//敵戦車
	m_enemyTank = std::make_unique<Tank>(nullptr, Vector3(0.0f, 0.0f, -10.0f), DirectX::XMConvertToRadians(180.0f));
	m_enemyTank->Initialize(IComponent::Type::ENEMY);

	m_enemyTank->SetOtherTank(m_playerTank.get());
	m_playerTank->SetOtherTank(m_enemyTank.get());

	// TPSカメラの生成
	m_tpsCamera = std::make_unique<mylib::FollowCamera>();
	m_tpsCamera->Initialize(m_playerTank.get());

	// コリジョンメッシュを生成する
	m_collisionMesh = std::make_unique<mylib::CollisionMesh>();
	m_collisionMesh->Initialize(device, context, L"Terrain");
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void PlayScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// デバッグカメラを更新する
	//m_debugCamera->Update(m_commonResources->GetInputManager());

	// ビュー行列を取得する
	//const Matrix& view = m_debugCamera->GetViewMatrix();

	// 戦車の更新処理
	Vector3 position(0.0f, 0.0f, 0.0f);
	float angle = 0.0f;
	m_playerTank->Update(elapsedTime,position,angle);
	m_enemyTank->Update(elapsedTime, position,angle);

	// フォローカメラを更新する
	m_tpsCamera->Update(elapsedTime);
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void PlayScene::Render()
{
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();

	// ビュー行列を取得する
	//const Matrix& view = m_debugCamera->GetViewMatrix();

	// ビュー行列の取得
	Matrix view = Matrix::CreateLookAt(
		m_tpsCamera->GetEyePosition(),
		m_tpsCamera->GetTargetPosition(),
		Vector3::UnitY
	);

	// ビュー行列を設定する
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

	//戦車の描画
	m_playerTank->Render();
	m_enemyTank->Render();

	// 当たり判定の表示
	/*Graphics::GetInstance()->GetPrimitiveBatch()->Begin();
	DX::Draw(Graphics::GetInstance()->GetPrimitiveBatch(), m_playerTank->GetBoundingSphere());
	DX::Draw(Graphics::GetInstance()->GetPrimitiveBatch(), m_enemyTank->GetBoundingSphere());
	Graphics::GetInstance()->GetPrimitiveBatch()->End();*/

	// キーボードステートの取得
	DirectX::Mouse::State mouseState = DirectX::Mouse::Get().GetState();
	float mousePosX = 1.0f / static_cast<float>(mouseState.x);
	float mousePosY = 1.0f / static_cast<float>(mouseState.y);

	// デバッグ情報を「DebugString」で表示する
	auto debugString = m_commonResources->GetDebugString();
	debugString->AddString("Play Scene");
	debugString->AddString(" ");
	debugString->AddString("PlayerTank");
	debugString->AddString("x : %f", m_playerTank->GetTankPosition().x);
	debugString->AddString("z : %f", m_playerTank->GetTankPosition().z);
	debugString->AddString("angle : %f", DirectX::XMConvertToDegrees(m_playerTank->GetTankAngleRL()));
	debugString->AddString(" ");
	debugString->AddString("EnemyTank");
	debugString->AddString("x : %f", m_enemyTank->GetTankPosition().x);
	debugString->AddString("z : %f", m_enemyTank->GetTankPosition().z);
	debugString->AddString("angle : %f", DirectX::XMConvertToDegrees(m_enemyTank->GetTankAngleRL()));
	debugString->AddString(" ");
	debugString->AddString("MousePosition");
	debugString->AddString("x : %d", mouseState.x);
	debugString->AddString("y : %d", mouseState.y);
	debugString->AddString("x : %f", mousePosX);
	debugString->AddString("y : %f", mousePosY);

	debugString->AddString("Hit : %d", m_enemyTank->GetHit());

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
		return IScene::SceneID::TITLE;
	}

	// シーン変更がない場合
	return IScene::SceneID::NONE;
}
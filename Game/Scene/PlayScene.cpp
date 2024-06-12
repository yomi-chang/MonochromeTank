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
#include "Libraries/MyLib/GridFloor.h"
#include "Libraries/MyLib/InputManager.h"
#include "Libraries/MyLib/MemoryLeakDetector.h"

#include "Framework/Resources.h"

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
	bodyModel{},

	m_angle{},
	m_bodyPosition{},
	m_turretPosition{},
	m_canonPosition{}
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
		0.1f, 100.0f
	);

	// モデルを読み込む
	/*bodyModel = DirectX::Model::CreateFromCMO(device, L"Resources/Models/TankBody.cmo", *fx);

	canonModel = DirectX::Model::CreateFromCMO(device, L"Resources/Models/TankCanon.cmo", *fx);

	turretModel = DirectX::Model::CreateFromCMO(device, L"Resources/Models/TankTurret.cmo", *fx);*/

	//Graphics::GetInstance()->Initialize();
	Resources::GetInstance()->LoadResource();
	bodyModel = Resources::GetInstance()->GetTankBodyModel();
	turretModel = Resources::GetInstance()->GetTankTurretModel();
	canonModel = Resources::GetInstance()->GetTankCanonModel();

	// 回転角を初期化する（度）
	m_angle = 0;
	m_canonAngle = 0;
	m_turretAngle = 0;

	// シーン変更フラグを初期化する
	m_isChangeScene = false;

	//速度の設定
	m_speed = 0.05f;

	//初期座標の設定
	m_bodyPosition = { 0.0f,0.5,0.0f };
	m_turretPosition = { 0.0f,1.0f,0.0f };
	m_canonPosition = { 0.0f,1.0f,-0.6f };
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void PlayScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	//キーボードイベントの処理
	this->KeyBoardEvent();

	// デバッグカメラを更新する
	m_debugCamera->Update(m_commonResources->GetInputManager());
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void PlayScene::Render()
{
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();

	// ビュー行列を取得する
	const Matrix& view = m_debugCamera->GetViewMatrix();
	// 格子床を描画する
	m_gridFloor->Render(context, view, m_projection);

	//車体
	Matrix world = Matrix::CreateScale(1.0f);

	world *= Matrix::CreateRotationY(m_angle);
	world *= Matrix::CreateTranslation(m_bodyPosition);

	bodyModel->Draw(context, *states, world, view, m_projection);

	//砲塔
	world = Matrix::CreateScale(1.0f);

	world *= Matrix::CreateTranslation(m_turretPosition);
	world *= Matrix::CreateRotationY(m_angle);
	turretModel->Draw(context, *states, world, view, m_projection);

	//砲身
	world = Matrix::CreateScale(1.0f);

	world *= Matrix::CreateTranslation(m_canonPosition);
	world *= Matrix::CreateRotationY(m_angle);
	canonModel->Draw(context, *states, world, view, m_projection);


	// デバッグ情報を「DebugString」で表示する
	auto debugString = m_commonResources->GetDebugString();
	debugString->AddString("Play Scene");
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


//---------------------------------------------------------
// キーボードの処理
//---------------------------------------------------------
void PlayScene::KeyBoardEvent()
{
	// キーボードステートを取得する
	DirectX::Keyboard::State keyboardState = DirectX::Keyboard::Get().GetState();

	//速度の初期化
	m_velocity = Vector3::Zero;

	// 前後移動
	if (keyboardState.Up)
	{
		m_velocity += Matrix::CreateRotationY(m_angle).Forward() * m_speed;
	}
	else if (keyboardState.Down)
	{
		m_velocity -= Matrix::CreateRotationY(m_angle).Forward() * m_speed;
	}
	// 左右回転
	if (keyboardState.Left)
	{
		m_angle += XMConvertToRadians(1.0f);
	}
	else if (keyboardState.Right)
	{
		m_angle -= XMConvertToRadians(1.0f);
	}

	m_bodyPosition += m_velocity;
	m_turretPosition += m_velocity;
	m_canonPosition += m_velocity;
}

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
	m_angle{}
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


	// モデルを読み込む準備
	std::unique_ptr<DirectX::EffectFactory> fx = std::make_unique<DirectX::EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");

	// モデルを読み込む
	bodyModel = DirectX::Model::CreateFromCMO(device, L"Resources/Models/Body.cmo", *fx);

	canonModel = DirectX::Model::CreateFromCMO(device, L"Resources/Models/canon.cmo", *fx);

	turretModel = DirectX::Model::CreateFromCMO(device, L"Resources/Models/Turret.cmo", *fx);

	// 回転角を初期化する（度）
	m_angle = 0;
	m_canonAngle = 0;
	m_turretAngle = 0;

	// シーン変更フラグを初期化する
	m_isChangeScene = false;
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void PlayScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// デバッグカメラを更新する
	m_debugCamera->Update(m_commonResources->GetInputManager());

	// 〇〇を回転する
	m_angle++;
	m_angle %= 360;


	// キーボードステートトラッカーを取得する
	const auto& kbTracker = m_commonResources->GetInputManager()->GetKeyboardTracker();

	// スペースキーが押されたら
	if (kbTracker->pressed.Space)
	{
		m_isChangeScene = true;
	}

	if (kbTracker->pressed.Down)
	{
		m_canonAngle += XMConvertToRadians(1.0f);
	}

	if (kbTracker->pressed.Down)
	{
		m_canonAngle += XMConvertToRadians(-1.0f);
	}

	if (kbTracker->pressed.Left)
	{
		m_turretAngle += XMConvertToRadians(-1.0f);
	}

	if (kbTracker->pressed.Right)
	{
		m_turretAngle += XMConvertToRadians(1.0f);
	}
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
	Matrix world = Matrix::CreateScale(0.01f);
	world *= Matrix::CreateTranslation({ 0,0.5,0 });
	world *= Matrix::CreateRotationY(XMConvertToRadians(static_cast<float>(m_angle)));
	bodyModel->Draw(context, *states, world, view, m_projection);
	
	//砲塔
	world = Matrix::CreateScale(0.01f);
	world *= Matrix::CreateRotationY(m_turretAngle);
	world *= Matrix::CreateRotationY(XMConvertToRadians(static_cast<float>(m_angle)));
	world *= Matrix::CreateTranslation({ 0,0.5,0 });
	turretModel->Draw(context, *states, world, view, m_projection);

	//砲身
	world = Matrix::CreateScale(0.01f);
	world *= Matrix::CreateRotationZ(m_canonAngle);
	world *= Matrix::CreateRotationY(XMConvertToRadians(static_cast<float>(m_angle)));
	world *= Matrix::CreateTranslation({ 0.1,0.5,0 });
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

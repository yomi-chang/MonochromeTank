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
#include "Libraries/Microsoft/RenderTexture.h"
#include "Libraries/MyLib/MemoryLeakDetector.h"

#include "Game/Objects/Stage/StageObject/SkySphere.h"
#include "Game/Objects/Stage/Wall.h"
#include "Game/Objects/Tank/PlayerTank.h"
#include "Game/Objects/Tank/EnemyTanks/EnemyTank.h"

#include "Game/UserInterface/MagazineUi.h"

#include "Game/Objects/Stage/StageManager.h"
#include "Game/Other/CollisionManager.h"
#include "Game/Scene/Fade.h"
#include "Game/Other/SharedData.h"

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
	m_renderTexture{},
	m_postProcess{},
	m_skipTexture{}
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
	// 戦車の生成
	CreateTanks();

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

	/*const auto& size = m_graphics->GetDeviceResources()->GetOutputSize();
	auto device = m_graphics->GetDeviceResources()->GetD3DDevice();

	m_renderTexture = std::make_unique<DX::RenderTexture>(DXGI_FORMAT_B8G8R8A8_UNORM);
	m_renderTexture->SetDevice(device);
	m_renderTexture->SetWindow(size);*/

	//　スキップテクスチャの受け取り
	m_skipTexture = Resources::GetInstance()->GetSkipTexture();

	//m_postProcess = std::make_unique<DirectX::BasicPostProcess>(device);
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

	// フェードイン中でフェードが終了しているならシーン遷移
	if (m_fade->GetFadeType() == Fade::FADEIN &&
		m_fade->FinishFade())
	{
		//生存している戦車情報をRetultDataに所有権ごと渡す
		if (!m_player->GetDead())
		{
			SharedData::GetInstance()->SetWinnerTank(m_player->ReleaseTank());
		}
		for (auto& enemy : m_enemies)
		{
			if (!enemy->GetDead())
				SharedData::GetInstance()->SetWinnerTank(enemy->ReleaseTank());
		}
		m_isChangeScene = true;
		return;
	}

	// フェードが終了していないなら早期リターン
	if (!m_fade->FinishFade()) { return; }

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

	// 生存戦車確認
	int surviveTank = 0;
	// プレイヤー
	if (!m_player->GetDead()){ surviveTank++;}
	// やられているならカメラ変更
	else{ m_cameraType = CameraType::DEBUG;}
	// 敵
	for (auto& enemy : m_enemies)
	{
		if (!enemy->GetDead()) { surviveTank++; }
	}

	// 生存している戦車が1台だけならフェード開始
	if (surviveTank == 1){ m_fade->FadeIn();}

	// キーボードステートの取得
	const auto& kbTracker = InputManager::GetInstance()->GetKeyboardTracker();
	
	// デバッグカメラでスペースキーを押したらフェード開始
	if (m_cameraType == CameraType::DEBUG &&
		kbTracker->IsKeyPressed(DirectX::Keyboard::Space))
	{
		m_fade->FadeIn();
	}

	// Cキーを押すことでデバッグカメラとTPSカメラを切り替える
	/*if (kbTracker->IsKeyPressed(DirectX::Keyboard::C))
	{
		this->ChangeCameraType();
	}*/
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void PlayScene::Render()
{
	// レンダーターゲットをオフスクリーンに
	/*auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
	auto rtv = m_renderTexture->GetRenderTargetView();
	auto defaultDSV = m_graphics->GetDeviceResources()->GetDepthStencilView();
	context->ClearRenderTargetView(rtv, DirectX::Colors::White);
	context->ClearDepthStencilView(defaultDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->OMSetRenderTargets(1, &rtv, defaultDSV);*/

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
	if (!m_player->GetDead())
	{
		m_magazine->Render();
	}
	else
	{
		// スキップUIの表示
		auto spriteBatch = m_graphics->GetSpriteBatch();
		RECT rect = { 1050,580,1260,700 };
		spriteBatch->Begin();
		spriteBatch->Draw(m_skipTexture, rect);
		spriteBatch->End();
	}
	
	
	// シーン遷移用
	m_fade->Render();

	// フレームバッファに切り替える
	// 描画先をフレームバッファに戻す 
	//auto defaultRTV = m_graphics->GetDeviceResources()->GetRenderTargetView();
	//context->OMSetRenderTargets(1, &defaultRTV, nullptr);
	// オフスクリーンの画像をSRVとして取得し、ポストプロセスをかける
	// ポストエフェクトをかける
	//auto srv = m_renderTexture->GetShaderResourceView();
	//m_postProcess->SetEffect(BasicPostProcess::Monochrome);
	//m_postProcess->SetSourceTexture(srv);
	//m_postProcess->Process(context);
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
// 戦車の生成
//---------------------------------------------------------
void PlayScene::CreateTanks()
{
	// プレイヤー戦車の生成
	m_player = std::make_unique<PlayerTank>(0, Vector3::Zero);
	m_player->Initialize();

	// 設定した戦車の数に応じた戦車の生成
	switch (SharedData::GetInstance()->GetTankCount())
	{
		case 1:
			m_enemies.push_back(std::make_unique<EnemyTank>(1, Vector3{ 0.0f, 0.0f, -8.0f }));
			break;
		case 2:
			m_enemies.push_back(std::make_unique<EnemyTank>(1, Vector3{ 0.0f, 0.0f, -8.0f }));
			m_enemies.push_back(std::make_unique<EnemyTank>(2, Vector3{ -5.0f, 0.0f, -5.0f }));
			break;
		case 3:
			m_enemies.push_back(std::make_unique<EnemyTank>(1, Vector3{ 0.0f, 0.0f, -8.0f }));
			m_enemies.push_back(std::make_unique<EnemyTank>(2, Vector3{ -5.0f, 0.0f, -5.0f }));
			m_enemies.push_back(std::make_unique<EnemyTank>(3, Vector3{ 3.0f, 0.0f, 5.0f }));
			break;
		default:
			break;
	}
	
	// 初期化処理
	for (auto& enemy : m_enemies)
	{
		enemy->Initialize();
	}
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



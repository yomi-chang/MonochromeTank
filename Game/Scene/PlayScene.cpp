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
#include "Libraries/MyLib/FollowCamera.h"
#include "Libraries/Microsoft/DebugDraw.h"
#include "Libraries/Microsoft/RenderTexture.h"
#include "Libraries/MyLib/MemoryLeakDetector.h"

#include "Game/Objects/Stage/StageObject/SkySphere.h"
#include "Game/Objects/Stage/Wall.h"
#include "Game/Objects/Tank/PlayerTank.h"
#include "Game/Objects/Tank/EnemyTanks/EnemyTank.h"

#include "Game/UserInterface/MagazineUi.h"
#include "Game/UserInterface/PauseMenu.h"

#include "Game/Objects/Stage/StageManager.h"
#include "Game/Other/CollisionManager.h"
#include "Game/Other/ProgressionManager.h"
#include "Game/Scene/Fade.h"
#include "Game/Particle/DamageEffect.h"
#include "Game/Particle/StageEffect.h"
#include "Game/Other/SharedData.h"
#include "Game/Other/Parameter.h"
#include "Game/Screen.h"

#include <cassert>

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
	m_progressionManager{},
	m_fade{},
	m_skipTexture{},
	m_damageEffect{},
	m_stageEffect{},
	m_pauseMenu{},
	m_time{},
	m_surviveTank{}
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
// 初期化処理
//---------------------------------------------------------
void PlayScene::Initialize()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	// 追加されている戦車情報の削除
	Messenger::GetInstance()->DestroyInstance();

	// BGMの再生
	SharedData::GetInstance()->GetSoundManager()->PlayBGM(XACT_WAVEBANK_SOUNDS_PLAYSCENE_BGM);

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

	// ダメージエフェクト
	m_damageEffect = std::make_unique<DamageEffect>();
	m_damageEffect->Create();

	// ステージエフェクト
	m_stageEffect = std::make_unique<StageEffect>();
	m_stageEffect->Create();

	// ポーズ画面
	m_pauseMenu = std::make_unique<PauseMenu>();

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
		m_stageManager->GetFixedTurrets(),
		m_stageManager->GetWalls(),
		m_stageManager->GetWallGimmick()
	);

	// 進行管理マネージャー
	m_progressionManager = std::make_unique<ProgressionManager>();
	m_progressionManager->Initialize(tankPointers);

	//　スキップテクスチャの受け取り
	m_skipTexture = Resources::GetInstance()->GetSkipTexture();
}

//---------------------------------------------------------
// 更新処理
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

	// ポーズ画面の更新(ポーズ中なら早期リターン)
	m_pauseMenu->Update(elapsedTime);
	if (m_pauseMenu->IsPause())
	{
		// シーン変更のフラグが立っていたらタイトルシーンに戻る
		if (m_pauseMenu->IsReturnTitle())
		{
			m_fade->FadeIn();
		}
		return; 
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

	// 生存戦車確認
	m_surviveTank = 0;
	// プレイヤー
	if (!m_player->GetDead()){ m_surviveTank++;}
	// やられているならカメラ変更
	else{ m_cameraType = CameraType::DEATH;}
	// 敵
	for (auto& enemy : m_enemies)
	{
		if (!enemy->GetDead()) { m_surviveTank++; }
	}

	// 生存している戦車が1台だけならフェード開始
	if (m_surviveTank == 1){ m_fade->FadeIn();}

	// キーボードステートの取得
	const auto& kbTracker = InputManager::GetInstance()->GetKeyboardTracker();
	
	// デバッグカメラでスペースキーを押したらフェード開始
	if (m_cameraType == CameraType::DEATH &&
		kbTracker->IsKeyPressed(DirectX::Keyboard::Space))
	{
		m_fade->FadeIn();
	}

	// 経過時間の設定
	m_stageEffect->SetElapsedTime(elapsedTime);

	// ダメージを食らったらヒットエフェクトを出す
	if (m_player->GetDamage())
		m_time = Parameter::GetInstance()->GetDamageEffectTime();

	// 時間経過
	if (m_time > 0)
		m_time -= elapsedTime;
}

//---------------------------------------------------------
// 描画処理
//---------------------------------------------------------
void PlayScene::Render()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

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
		case CameraType::DEATH:
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
	
	// UIの描画
	this->DrawUi();

	// ステージエフェクト
	m_stageEffect->Render();

	// ダメージエフェクト
	if (m_time > 0)
	{
		m_damageEffect->SetElapsedTime(m_time);
		m_damageEffect->Render();
	}

	// ポーズ画面の描画
	m_pauseMenu->Render();
	
	// シーン遷移用
	m_fade->Render();
}

//---------------------------------------------------------
// 終了処理
//---------------------------------------------------------
void PlayScene::Finalize()
{
	// do nothing.
}

//---------------------------------------------------------
// 次のシーンIDの取得
//---------------------------------------------------------
IScene::SceneID PlayScene::GetNextSceneID() const
{
	// シーン変更がある場合
	if (m_isChangeScene)
	{	
		// ポーズ画面の場合タイトルに
		if (m_pauseMenu->IsPause())
		{
			return IScene::SceneID::TITLE;
		}
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
	// パラメータの設定
	auto parameter = Parameter::GetInstance();

	// プレイヤー戦車の生成
	m_player = std::make_unique<PlayerTank>(0, parameter->GetPlayerPosition());
	m_player->Initialize();

	// 設定した戦車の数に応じた戦車の生成
	switch (SharedData::GetInstance()->GetTankCount())
	{
		case 1:
			m_enemies.push_back(std::make_unique<EnemyTank>(1, parameter->GetEnemyPosition(0)));
			break;
		case 2:
			m_enemies.push_back(std::make_unique<EnemyTank>(1, parameter->GetEnemyPosition(0)));
			m_enemies.push_back(std::make_unique<EnemyTank>(2, parameter->GetEnemyPosition(1)));
			break;
		case 3:
			m_enemies.push_back(std::make_unique<EnemyTank>(1, parameter->GetEnemyPosition(0)));
			m_enemies.push_back(std::make_unique<EnemyTank>(2, parameter->GetEnemyPosition(1)));
			m_enemies.push_back(std::make_unique<EnemyTank>(3, parameter->GetEnemyPosition(2)));
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
// Uiの描画
//---------------------------------------------------------
void PlayScene::DrawUi()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	auto spriteBatch = m_graphics->GetSpriteBatch();

	// UI関係
	if (!m_player->GetDead())
	{
		m_magazine->Render();
	}
	else
	{
		spriteBatch->Begin();
		// スキップUIの描画
		spriteBatch->Draw(
			m_skipTexture,
			Vector2(Screen::CENTER_X + 500, Screen::CENTER_Y + 300),
			nullptr,
			Colors::White,
			0.0f,
			mylib::GetTextureCenter(m_skipTexture),
			0.5f
		);
		spriteBatch->End();
	}

	spriteBatch->Begin();
	// 残り戦車の表示
	spriteBatch->Draw(
		Resources::GetInstance()->GetTankCountTexture(),
		Vector2(Screen::CENTER_X + 420, Screen::CENTER_Y - 300),
		nullptr,
		DirectX::Colors::White,
		0.0f,
		mylib::GetTextureCenter(Resources::GetInstance()->GetTankCountTexture()),
		0.5f
	);

	RECT rect = { 0,0,0,0 };
	rect.left = (m_surviveTank - 2) * FONT_SIZE_X;
	rect.right = rect.left + FONT_SIZE_X;
	rect.bottom = FONT_SIZE_Y;
	spriteBatch->Draw(
		Resources::GetInstance()->GetCountTextTexture(),
		Vector2(Screen::CENTER_X + 600, Screen::CENTER_Y - 300),
		&rect,
		DirectX::Colors::White,
		0.0f,
		mylib::GetTextureCenter(Resources::GetInstance()->GetCountTextTexture()),
		0.5f
	);
	spriteBatch->End();

}

/**
 * @file   PlayScene.cpp
 * @brief  プレイシーンクラス
 */
#include "pch.h"
#include "PlayScene.h"

#include "Framework/InputManager.h"

#include "Libraries/MyLib/DebugCamera.h"
#include "Libraries/MyLib/LockOnCamera.h"
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
#include "Game/UserInterface/Button.h"

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

/// <summary>
/// コンストラクタ
/// </summary>
PlayScene::PlayScene()
	:
	m_graphics{ Graphics::GetInstance() },
	m_deathCamera{},
	m_debugCamera{},
	m_tpsCamera{},
	m_cameraType{ CameraType::TPS },
	m_isChangeScene{},
	m_player{},
	m_enemies{},
	m_allTanks{},
	m_stageManager{},
	m_collisonManager{},
	m_progressionManager{},
	m_fade{},
	m_skipTexture{},
	m_damageEffect{},
	m_stageEffect{},
	m_pauseMenu{},
	m_time{},
	m_surviveEnemyTank{},
	m_buttons{}
{
}

/// <summary>
/// コンストラクタ
/// </summary>
PlayScene::~PlayScene()
{
	// do nothing.
}

/// <summary>
/// 初期化処理
/// </summary>
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
	// デバッグカメラの作成
	m_debugCamera = std::make_unique<mylib::DebugCamera>();
	m_debugCamera->Initialize(rect.right, rect.bottom);

	// 射影行列の作成
	Matrix projection;
	projection = Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(65.0f),
		static_cast<float>(rect.right) / static_cast<float>(rect.bottom),
		0.1f, 1000.0f
	);
	// 射影行列の設定
	m_graphics->SetProjectionMatrix(projection);

	// シーン変更フラグを初期化する
	m_isChangeScene = false;
	// マウスの固定
	InputManager::GetInstance()->LockMouseCursor();

	// オブジェクトの生成
	this->CreateObjects();
	// ボタンの作成
	this->CreateButton();

	// スキップテクスチャの受け取り
	m_skipTexture = Resources::GetInstance()->GetSkipTexture();
}

/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">フレーム間の経過時間</param>
void PlayScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// シーン遷移用
	m_fade->Update(elapsedTime);

	// マウスの制限解除
	this->UnDisableMouse();

	// デバッグカメラを更新する
	m_debugCamera->Update();
	// フォローカメラを更新する
	m_tpsCamera->Update(elapsedTime);
	// デスカメラを更新する
	m_deathCamera->Update(elapsedTime);

	// フェードイン中でフェードが終了しているならシーン遷移
	if (m_fade->GetFadeType() == Fade::FADEIN && m_fade->FinishFade())
	{
		//生存している戦車情報をResultDataに所有権ごと渡す
		if (!m_player->GetDead())
			SharedData::GetInstance()->SetWinnerTank(m_player->ReleaseTank());
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
		// タイトルに戻るが押されたらフェード開始
		if (m_pauseMenu->IsReturnTitle()) { m_fade->FadeIn(); }
		return; 
	}

	// コリジョンマネージャーの更新
	m_collisonManager->Update();
	// 進行管理マネージャーの更新
	m_progressionManager->Update();
	// 戦車の更新
	for (auto& tank : m_allTanks)
	{
		tank->Update(elapsedTime);
	}
	// ステージの更新
	m_stageManager->Update(elapsedTime);

	// 生存している敵戦車の数を調べる
	m_surviveEnemyTank = 0;
	for (auto& enemy : m_enemies)
	{
		if (!enemy->GetDead()) { m_surviveEnemyTank++; }
	}
	// プレイヤー
	if (m_player->GetDead())
	{ 
		// 進行管理マネージャーに生存している敵戦車の数を渡す
		m_progressionManager->SetTankCount(m_surviveEnemyTank);
		// デスカメラに変更
		m_cameraType = CameraType::DEATH; 
		m_player->GetTank()->GetCannon()->SetDisplaySight(false);
		// 生存している敵戦車が残り1台なら終了
		if(m_surviveEnemyTank == 1) { m_fade->FadeIn(); }
	}
	else if (m_surviveEnemyTank == 0)
	{
		// 生存している敵戦車がいないならクリア
		m_fade->FadeIn();
	}

	// デスカメラの場合の処理
	if (m_cameraType == CameraType::DEATH)
	{
		// マウスの固定解除
		InputManager::GetInstance()->UnLockMouseCursor();

		// ボタンの判定
		for (auto& button : m_buttons)
		{
			button->CheckOnMouseOver();
			button->CheckClickButton();
		}
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

/// <summary>
/// 描画処理
/// </summary>
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
			view = Matrix::CreateLookAt(
				m_deathCamera->GetEyePosition(),
				m_deathCamera->GetTargetPosition(),
				Vector3::UnitY
			);
			Graphics::GetInstance()->SetViewMatrix(view);
			break;
		case  CameraType::DEBUG:
			view = m_debugCamera->GetViewMatrix();
			Graphics::GetInstance()->SetViewMatrix(view);
			break;
	}
	Graphics::GetInstance()->SetViewMatrix(view);

	// ステージの描画
	m_stageManager->Render();

	// 戦車の描画
	for (auto& tank : m_allTanks)
	{
		tank->Render();
	}
	// 照準の描画
	m_player->DrawSight();
	// 弾と体力ゲージの描画
	for (auto& tank : m_allTanks)
	{
		tank->DrawBullet();
		tank->DrawHpGauge();
	}
	
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

/// <summary>
/// 終了処理
/// </summary>
void PlayScene::Finalize()
{
	// do nothing.
}

/// <summary>
/// 次のシーンIDの取得
/// </summary>
/// <returns>シーンID</returns>
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

/// <summary>
/// マウスの制限解除
/// </summary>
void PlayScene::UnDisableMouse()
{
	// マウスが制限されている場合
	auto inputManager = InputManager::GetInstance();
	if (inputManager->GetDisableMouseClick())
	{
		// マウスの左ボタンから指が離れているなら
		auto& mouseState = inputManager->GetMouseState();
		if (!mouseState.leftButton)
		{
			inputManager->SetDisableMouseClick(false);
		}
	}
}

/// <summary>
/// オブジェクトの生成
/// </summary>
void PlayScene::CreateObjects()
{
	using namespace DirectX::SimpleMath;

	// オブジェクトの生成============================================================
	// 戦車の生成
	this->CreateTanks();
	// TPSカメラの生成
	m_tpsCamera = std::make_unique<mylib::FollowCamera>();
	m_tpsCamera->Initialize(m_player->GetTank());
	// デスカメラの生成
	m_deathCamera = std::make_unique<mylib::LockOnCamera>();
	m_deathCamera->Initialize();
	m_deathCamera->SetTargetPosition(Vector3::Zero);
	m_deathCamera->SetDistance(1.0f);
	m_deathCamera->SetHeight(17.0f);
	m_deathCamera->SetEyePosition(Vector3(0.0f, 5.0f, 0.0f));
	// ステージマネージャーの生成
	m_stageManager = std::make_unique<StageManager>();
	m_stageManager->Initialize();
	// UI関係
	m_magazine = std::make_unique<MagazineUi>();
	m_magazine->Initialize();
	m_magazine->SetPlayer(m_player.get());
	// コリジョンマネージャー
	m_collisonManager = std::make_unique<CollisionManager>();
	// 進行管理マネージャー
	m_progressionManager = std::make_unique<ProgressionManager>();
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
	m_pauseMenu->Initialize();
	// 全戦車の情報を持つ配列
	std::vector<Tank*> tankPointers;
	for (auto& tank : m_allTanks)
	{
		tankPointers.push_back(tank->GetTank());
	}
	// 全敵戦車情報を持つ配列
	std::vector<EnemyTank*> enemyTankPointers;
	for (auto& enemy : m_enemies)
	{
		enemyTankPointers.push_back(enemy.get());
	}

	// 必要な情報の設定============================================================
	// 各戦車に全戦車の情報を設定
	for (auto& tank : m_allTanks)
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
	// 進行管理マネージャーに敵戦車情報の設定
	m_progressionManager->Initialize(enemyTankPointers);
}

/// <summary>
/// ボタンの作成
/// </summary>
void PlayScene::CreateButton()
{
	using namespace DirectX::SimpleMath;

	// ゲーム終了ボタン
	auto battleSkipButton = std::make_unique<Button>();
	battleSkipButton->Initialize(
		Resources::GetInstance()->GetSkipTexture(),
		0.7f,
		Vector2(Screen::CENTER_X + 500, Screen::CENTER_Y + 300)
	);
	// クリック時の処理
	battleSkipButton->SetOnClick([this]() {
		// 戦闘の終了
		m_fade->FadeIn();
		// SEの再生
		SharedData::GetInstance()->GetSoundManager()->PlaySE(XACT_WAVEBANK_SOUNDS_BUTTON_SE);
	});

	// ボタン情報配列に譲渡する
	m_buttons.emplace_back(std::move(battleSkipButton));
}

/// <summary>
/// 戦車の生成
/// </summary>
void PlayScene::CreateTanks()
{
	// パラメータの設定
	auto parameter = Parameter::GetInstance();

	// プレイヤー戦車の生成
	m_player = std::make_unique<PlayerTank>(0, parameter->GetPlayerPosition());

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

	// 全戦車の登録
	m_allTanks.push_back(m_player.get());
	for (auto& enemy : m_enemies)
	{
		m_allTanks.push_back(enemy.get());
	}

	// 初期化処理
	for (auto& tank : m_allTanks) 
	{
		tank->Initialize();
	}
}

/// <summary>
/// UIの描画
/// </summary>
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
		// ボタンの描画
		for (auto& button : m_buttons)
		{
			button->Render();
		}
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
	rect.left = (m_surviveEnemyTank - 1) * FONT_SIZE_X;
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

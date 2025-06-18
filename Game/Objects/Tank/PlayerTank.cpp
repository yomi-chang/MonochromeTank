/**
 * @file   PlayerTank.cpp
 * @brief  自機クラス
 */
#include "pch.h"
#include "Game/Objects/Tank/PlayerTank.h"

#include "Game/Objects/Tank/TankBase/Tank.h"
#include "Game/Objects/Tank/TankBase/TankBody.h"
#include "Game/Objects/Tank/TankBase/TankTurret.h"
#include "Game/Objects/Tank/TankBase/TankCannon.h"

#include "Interface/IBullet.h"
#include "Game/UserInterface/HpGauge.h"

#include "Framework/InputManager.h"
#include "Game/Other/Parameter.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="tankNumber">戦車番号</param>
/// <param name="position">座標</param>
PlayerTank::PlayerTank(
	int tankNumber,
	const DirectX::SimpleMath::Vector3& position
)
	:
	m_tankNumber{ tankNumber },
	m_position{ position },
	m_angle{},
	m_tank{},
	m_hpGauge{},
	m_camera{},
	m_previousHp{},
	m_isDamage{}
{
}

/// <summary>
/// デストラクタ
/// </summary>
PlayerTank::~PlayerTank()
{
}

/// <summary>
/// 初期化処理
/// </summary>
void PlayerTank::Initialize()
{
	using namespace DirectX::SimpleMath;

	// 戦車の生成
	m_tank = std::make_unique<Tank>(m_tankNumber,m_position, 0.0f);
	m_tank->Initialize();
	m_tank->SetMaxHp(Parameter::GetInstance()->GetPlayerHp());

	// 座標の設定
	m_position = m_tank->GetPosition();

	// HPゲージの作成
	m_hpGauge = std::make_unique<HpGauge>();
	m_hpGauge->Initialize(Vector2{ 200,50 });
	m_previousHp = m_tank->GetHp();
}

/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">フレーム間の経過時間</param>
void PlayerTank::Update(float elapsedTime)
{
	// ダメージを受けたかどうかのリセット
	m_isDamage = false;

	// 破壊されていたら処理しない
	if (m_tank->GetDead()) { return; }

	// キー入力の処理
	this->KeyBoardEvent(elapsedTime);
	// 戦車の更新
	m_tank->Update(elapsedTime);

	// 座標の更新
	m_position = m_tank->GetPosition();

	// 座標と回転角の更新
	m_position = m_tank->GetPosition();
	m_angle = m_tank->GetRotation();

	// ダメージを受けていたらカメラを揺らす
	if (m_previousHp != m_tank->GetHp())
	{
		m_isDamage = true;
		m_camera->StartShakeCamera(15.0f, 0.05f, 0.4f);
	}
	m_previousHp = m_tank->GetHp();
}

/// <summary>
/// 描画処理
/// </summary>
void PlayerTank::Render()
{
	// 戦車の描画
	m_tank->Render();
}

/// <summary>
/// 終了処理
/// </summary>
void PlayerTank::Finalize()
{
}

/// <summary>
/// キーボードイベント
/// </summary>
/// <param name="elapsedTime">フレーム間の経過時間</param>
void PlayerTank::KeyBoardEvent(float elapsedTime)
{
	using namespace DirectX::SimpleMath;
	using namespace DirectX;

	// 破壊されているなら早期リターン
	if (m_tank->GetHp() <= 0) { return; }

	// 移動処理
	this->Move(elapsedTime);

	// 砲塔と砲身の回転
	this->RotateTurretCannon();

	// マウスが制限されているなら早期リターン
	if (InputManager::GetInstance()->GetDisableMouseClick()) { return; }

	// 弾の発射
	const auto& mouseState = InputManager::GetInstance()->GetMouseState();
	if (mouseState.leftButton)
	{
		// 砲弾が発射されたときのみカメラを揺らす
		if (m_tank->GetCannon()->GetCurrentBullet() == TankCannon::BulletType::CANNONBALL &&
			m_tank->GetCannon()->GetCannonBall()->GetBulletState() == IBullet::UNUSED)
		{
			m_camera->StartShakeCamera(25.0f, 0.05f, 0.5f);
		}
		// 発射
		m_tank->GetCannon()->Shoot();
	}
	else
	{
		// 発射終了
		m_tank->GetCannon()->FinishShoot();
	}

	// 弾の変更
	const auto& keyboardTracker = InputManager::GetInstance()->GetKeyboardTracker();
	if (keyboardTracker->IsKeyPressed(DirectX::Keyboard::Space))
	{
		m_tank->GetCannon()->ChangeBullet();
	}

	// リロード
	const auto& mouseTracker = InputManager::GetInstance()->GetMouseTracker();
	if (mouseTracker->rightButton == mouseTracker->PRESSED)
	{
		m_tank->GetCannon()->StartReload();
	}
}

/// <summary>
/// 移動処理
/// </summary>
/// <param name="elapsedTime">フレーム間の経過時間</param>
void PlayerTank::Move(float elapsedTime)
{
	using namespace DirectX::SimpleMath;
	using namespace DirectX;

	// キーボードステートの取得
	const auto& keyboardState = InputManager::GetInstance()->GetKeyboardState();
	// パラメータの取得
	const auto& parameter = Parameter::GetInstance();

	// 変数宣言
	Vector3 velocity = Vector3::Zero;
	float speed = parameter->GetPlayerSpeed() * elapsedTime;
	float angle = 0.0f;

	// 左右回転
	if (keyboardState.A)
		angle = DirectX::XMConvertToRadians(parameter->GetPlayerRotationSpeed());
	else if (keyboardState.D)
		angle = DirectX::XMConvertToRadians(-parameter->GetPlayerRotationSpeed());

	// 前後移動
	if (keyboardState.W)
		velocity = Vector3::Transform(Vector3::Forward * speed, m_tank->GetRotation());
	else if (keyboardState.S)
		velocity = Vector3::Transform(Vector3::Backward * speed, m_tank->GetRotation());

	// 同時押しされていたら停止
	if (keyboardState.A && keyboardState.D)
		angle = 0.0f;
	if (keyboardState.W && keyboardState.S)
		velocity = Vector3::Zero;
		
	// 移動もしくは回転をしていないなら早期リターン
	if (angle == 0.0f && velocity == Vector3::Zero) { return; }

	// 移動させる
	m_tank->GetBody()->Move(velocity);
	// 回転させる
	m_tank->GetBody()->Rotate(Quaternion::CreateFromYawPitchRoll(angle, 0.0f, 0.0f));
}

/// <summary>
/// 砲塔と砲身の回転
/// </summary>
void PlayerTank::RotateTurretCannon()
{
	// マウスステートの取得
	const auto& mouseState = InputManager::GetInstance()->GetMouseState();
	// パラメータの取得
	const auto& parameter = Parameter::GetInstance();

	// 砲塔の回転量
	float rotationY = static_cast<float>(mouseState.x) * 0.001f;
	// 砲身の回転量
	float rotationX = static_cast<float>(mouseState.y) * 0.001f;

	// 回転していないなら早期リターン
	if (rotationY == 0.0f && rotationX == 0.0f) { return; }

	// 砲塔角度をオイラー角に変換
	float eulerAngle = m_tank->GetTurretRotation().ToEuler().y - rotationY;
	// 砲塔回転の制限
	float min = parameter->GetTurretAngleMin();
	float max = parameter->GetTurretAngleMax();
	eulerAngle = mylib::Clamp(eulerAngle, min, max);
	// 回転情報を砲塔に伝える
	m_tank->GetTurret()->RotateTurret(eulerAngle);
	
	// 砲身角度をオイラー角に変換(受け取った値を変換)
	eulerAngle = m_tank->GetCannonRotation().ToEuler().x - rotationX;
	// 砲身回転の制限
	min = parameter->GetCannonAngleMin();
	max = parameter->GetCannonAngleMax();
	eulerAngle = mylib::Clamp(eulerAngle, min, max);
	// 回転情報を砲身に伝える
	m_tank->GetCannon()->RotateCannon(eulerAngle);
}

/// <summary>
/// 座標の受け渡し
/// </summary>
/// <param name="position">座標</param>
void PlayerTank::SetPosition(const DirectX::SimpleMath::Vector3& position)
{
	m_tank->GetBody()->SetCollisionVel(position);
}

/// <summary>
/// 砲身情報の取得
/// </summary>
/// <returns>砲身情報</returns>
TankCannon* PlayerTank::GetTankCannon() 
{
	return m_tank->GetCannon(); 
}

/// <summary>
/// メッセージの受け取り
/// </summary>
/// <param name="messageID">メッセージ</param>
void PlayerTank::OnMessegeAccepted(Message::MessageID messageID)
{
	UNREFERENCED_PARAMETER(messageID);
}

/// <summary>
/// 体力ゲージの描画
/// </summary>
void PlayerTank::DrawHpGauge()
{
	// HPゲージ
	if (m_tank->GetHp() <= 0) { return; }
	m_hpGauge->Render(m_tank->GetHpRatio());
}

/// <summary>
/// 弾の描画
/// </summary>
void PlayerTank::DrawBullet()
{
	// 弾の描画
	if (m_tank->GetHp() <= 0) { return; }
	m_tank->GetCannon()->DrawBullet();
}

/// <summary>
/// 他戦車情報の設定
/// </summary>
/// <param name="tanks">全戦車情報</param>
void PlayerTank::SetOtherTanks(std::vector<Tank*> tanks)
{
	m_tank->SetOtherTanks(tanks);
}


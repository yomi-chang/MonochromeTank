#include "pch.h"
#include "Game/Objects/NewTank/PlayerTank.h"

#include "Game/Objects/NewTank/NewTankBase/NewTank.h"
#include "Game/Objects/NewTank/NewTankBase/NewTankBody.h"
#include "Game/Objects/NewTank/NewTankBase/NewTankTurret.h"
#include "Game/Objects/NewTank/NewTankBase/NewTankCannon.h"

#include "Game/Collider/SphereCollider.h"
#include "Game/UserInterface/HpGauge.h"

#include "Framework/InputManager.h"
#include "Libraries/MyLib/Math.h"
#include "Libraries/MyLib/DebugLog.h"

PlayerTank::PlayerTank()
{
}

PlayerTank::~PlayerTank()
{
}

void PlayerTank::Initialize()
{
	using namespace DirectX::SimpleMath;

	// 戦車の生成
	Vector3 initialPosition = Vector3::Zero;
	m_tank = std::make_unique<NewTank>(initialPosition, 0.0f);
	m_tank->Initialize();

	// コライダーの作成
	m_collider = std::make_unique<SphereCollider>();
	m_collider->CreateBoundingSphere(m_tank->GetBody()->GetPosition(), 1.0f);

	// HPゲージの作成
	m_hpGauge = std::make_unique<HpGauge>();
	m_hpGauge->Initialize(Vector2{ 200,50 });
}

void PlayerTank::Update(float elapsedTime)
{
	KeyBoardEvent(elapsedTime);

	// 戦車の更新
	m_tank->Update(elapsedTime);

	// 座標と回転角の更新
	m_position = m_tank->GetBody()->GetPosition();
	m_angle = m_tank->GetBody()->GetAngle();

	// コライダーの更新
	m_collider->Update(m_position);

	// ダメージの初期化
	m_damege = 0.0f;
	// 衝突判定
	// ダメージ処理
	m_hpGauge->Damage(m_damege);
}

void PlayerTank::Render()
{
	// 戦車の描画
	m_tank->Render();

	// コライダーの描画
	m_collider->Render();

	// HPゲージ
	m_hpGauge->Render();
}

void PlayerTank::Finalize()
{
}

void PlayerTank::Attach(std::unique_ptr<IObject> parts)
{
}

void PlayerTank::Detach(std::unique_ptr<IObject> parts)
{
}


// キーボードイベント
void PlayerTank::KeyBoardEvent(float elapsedTime)
{
	using namespace DirectX::SimpleMath;
	using namespace DirectX;

	// 移動処理
	Move(elapsedTime);

	// 砲塔と砲身の回転
	RotateTurretCannon();

	// 弾の発射
	const auto& mouseState = InputManager::GetInstance()->GetMouseState();
	if (mouseState.leftButton)
	{
		m_tank->GetCannon()->Shoot();
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

// 移動処理
void PlayerTank::Move(float elapsedTime)
{
	using namespace DirectX::SimpleMath;
	using namespace DirectX;

	// キーボードステートの取得
	const auto& keyboardState = InputManager::GetInstance()->GetKeyboardState();

	// 変数宣言
	Vector3 velocity = Vector3::Zero;
	float speed = elapsedTime * 3.0f;
	Quaternion angle = Quaternion::Identity;

	// 前後移動
	if (keyboardState.W)
		velocity += Vector3::Transform(Vector3::Forward * speed, m_tank->GetBody()->GetAngle());
	else if (keyboardState.S)
		velocity += Vector3::Transform(Vector3::Backward * speed, m_tank->GetBody()->GetAngle());

	// 移動させる
	m_tank->GetBody()->Move(velocity);

	// 左右回転
	if (keyboardState.A)
		m_tank->GetBody()->Rotate(Quaternion::CreateFromYawPitchRoll(XMConvertToRadians(0.5f), 0.0f, 0.0f));
	else if (keyboardState.D)
		m_tank->GetBody()->Rotate(Quaternion::CreateFromYawPitchRoll(XMConvertToRadians(-0.5f), 0.0f, 0.0f));
}

// 砲塔と砲身の回転
void PlayerTank::RotateTurretCannon()
{
	// マウスステートの取得
	const auto& mouseState = InputManager::GetInstance()->GetMouseState();

	// 砲塔の回転量
	float rotationY = static_cast<float>(mouseState.x) * 0.001f;
	// 砲塔角度をオイラー角に変換
	float eulerAngle = m_tank->GetTurret()->GetTurretAngle().ToEuler().y - rotationY;
	// 砲塔回転の制限
	eulerAngle = mylib::Clamp(eulerAngle, TURRET_ANGLE_MIN, TURRET_ANGLE_MAX);
	// 回転情報を砲塔に伝える
	m_tank->GetTurret()->RotateTurret(eulerAngle);

	// 砲身の回転量
	float rotationX = static_cast<float>(mouseState.y) * 0.001f;
	// 砲身角度をオイラー角に変換(受け取った値を変換)
	eulerAngle = m_tank->GetCannon()->GetCannonAngle().ToEuler().x - rotationX;
	// 砲身回転の制限
	eulerAngle = mylib::Clamp(eulerAngle, CANNON_ANGLE_MIN, CANNON_ANGLE_MAX);
	// 回転情報を砲身に伝える
	m_tank->GetCannon()->RotateCannon(eulerAngle);
}

void PlayerTank::SetWalls(std::vector<Wall*> walls)
{
	m_tank->GetCannon()->SetWalls(walls);
}

void PlayerTank::SetPosition(DirectX::SimpleMath::Vector3 position)
{
	m_tank->GetBody()->SetCollisionVel(position);
}

//std::vector<std::unique_ptr<IBullet>>& PlayerTank::GetBullets()
//{
//	return m_tank->GetCannon()->GetBullets();
//}
//
//std::unique_ptr<IBullet>& PlayerTank::GetCannonBall()
//{
//	return m_tank->GetCannon()->GetCannonBall();
//}

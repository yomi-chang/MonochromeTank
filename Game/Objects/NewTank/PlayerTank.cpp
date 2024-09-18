#include "pch.h"
#include "Game/Objects/NewTank/PlayerTank.h"

#include "Game/Objects/NewTank/NewTankBase/NewTank.h"
#include "Game/Objects/NewTank/NewTankBase/NewTankBody.h"
#include "Game/Objects/NewTank/NewTankBase/NewTankTurret.h"
#include "Game/Objects/NewTank/NewTankBase/NewTankCannon.h"

#include "Game/Collider/SphereCollider.h"

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
	m_collider->CreateBoundingSphere(initialPosition, 1.0f);
}

void PlayerTank::Update(float elapsedTime)
{
	Move(elapsedTime);
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

	// 戦車の更新
	m_tank->Update(elapsedTime);

	// コライダーの更新
	m_collider->Update(m_tank->GetBody()->GetPosition());
}

void PlayerTank::Render()
{
	// 戦車の描画
	m_tank->Render();

	// コライダーの描画
	m_collider->Render();
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

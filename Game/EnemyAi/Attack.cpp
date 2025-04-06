/*
	@file	Attack.cpp
	@brief	敵の攻撃処理クラス
*/
#include "pch.h"
#include "Game/EnemyAi/Attack.h"
#include "Libraries/MyLib/Math.h"

//-------------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------------
Attack::Attack()
	:
	m_tank{},
	m_targetTank{},
	m_time{},
	m_moveTime{},
	m_shotTime{}
{
}

//-------------------------------------------------------------------
// 初期化処理
//-------------------------------------------------------------------
void Attack::Initialize(Tank* tank)
{
	m_tank = tank;
	m_currentAction = Action::SHOT;

	// 初期の移動時間の設定
	m_moveTime = mylib::Random(1.0f, MOVE_TIME);

	// 初期の攻撃時間の設定
	m_shotTime = mylib::Random(1.0f, SHOT_TIME);
}

//-------------------------------------------------------------------
// 更新処理
//-------------------------------------------------------------------
void Attack::Update(float elapsedTime)
{
	// 追跡対象の戦車がいないなら処理しない
	if (m_targetTank == nullptr) { return; }

	// 追跡対象の方向を向く
	LookTargetTank(elapsedTime);

	// 射撃処理
	m_tank->GetCannon()->StartReload();
	m_tank->GetCannon()->Shoot();
	m_tank->GetCannon()->FinishShoot();

	// 行動
	switch (m_currentAction)
	{
		case Attack::SHOT:
			ShotAction(elapsedTime);
			break;
		case Attack::MOVE:
			MoveAction(elapsedTime);
			break;
		default:
			break;
	}
}

//-------------------------------------------------------------------
// 追跡対象の方向を向く
//-------------------------------------------------------------------
void Attack::LookTargetTank(float elapsedTime)
{
	using namespace DirectX::SimpleMath;

	// パラメータの受け取り
	const auto& parameter = Parameter::GetInstance();

	// 敵の方向ベクトルの計算
	Vector3 delta = m_tank->GetPosition() - m_targetTank->GetPosition();
	float angleRadians = atan2(delta.x, delta.z);

	// 車体の回転を考慮して目標の角度を計算
	float targetAngle = angleRadians - m_tank->GetRotation().ToEuler().y;

	// 砲塔回転の制限
	float min = parameter->GetTurretAngleMin();
	float max = parameter->GetTurretAngleMax();
	targetAngle = mylib::Clamp(targetAngle, DirectX::XMConvertToRadians(min), DirectX::XMConvertToRadians(max));

	// 現在の砲塔の回転角度
	float currentAngle = m_tank->GetTurret()->GetTurretRotation().ToEuler().y;

	// 目標角度と現在の角度との差を求め、Lerp補間で回転
	float angleDifference = targetAngle - currentAngle;

	// ゆっくり回転するための速度制御
	float rotationSpeed = parameter->GetEnemyRotationSpeed() * elapsedTime;

	// 補間後の回転角度
	float newAngle = currentAngle + angleDifference * rotationSpeed;

	// 砲塔の回転
	m_tank->GetTurret()->RotateTurret(newAngle);
}

//-------------------------------------------------------------------
// 移動
//-------------------------------------------------------------------
void Attack::MoveAction(float elapsedTime)
{
	using namespace DirectX::SimpleMath;

	// 時間経過
	m_time += elapsedTime;
	if (m_time >= m_moveTime)
	{
		m_time = 0.0f;
		m_currentAction = Action::SHOT;
		// 移動時間を再度設定
		m_moveTime = mylib::Random(1.0f, MOVE_TIME);
	}

	// 移動
	Vector3 velocity = Vector3::Transform(Vector3::Forward * (TANK_SPEED * elapsedTime), m_tank->GetRotation());

	// 移動処理
	m_tank->GetBody()->Move(velocity);
}

//-------------------------------------------------------------------
// 射撃
//-------------------------------------------------------------------
void Attack::ShotAction(float elapsedTime)
{
	m_time += elapsedTime;
	if (m_time >= m_shotTime)
	{
		m_time = 0.0f;
		m_currentAction = Action::MOVE;
		// 初期の攻撃時間の設定
		m_shotTime = mylib::Random(1.0f, SHOT_TIME);
	}

	// 射撃処理
	m_tank->GetCannon()->StartReload();
	m_tank->GetCannon()->Shoot();
}

//-------------------------------------------------------------------
// 追跡対象の戦車が離れている
//-------------------------------------------------------------------
void Attack::IsTargetTankFar()
{
	// 追跡対象の戦車との距離を調べる
	float distance = (m_targetTank->GetPosition() - m_tank->GetPosition()).LengthSquared();

	// 追跡対象の戦車に接近したなら攻撃行動開始
	if (distance <= 10.0f)
	{
		// 追跡行動にする
		Messenger::GetInstance()->Dispatch(m_tank->GetTankNumber(), Message::TRACKING);
	}
}

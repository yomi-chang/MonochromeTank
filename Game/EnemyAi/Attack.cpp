/**
 * @file   Attack.cpp
 * @brief  敵の攻撃処理クラス
 */
#include "pch.h"
#include "Game/EnemyAi/Attack.h"

/// <summary>
/// コンストラクタ
/// </summary>
Attack::Attack()
	:
	m_stateID{ StateID::ATTACK },
	m_tank{},
	m_targetTank{},
	m_moveTime{},
	m_shotTime{},
	m_currentAction{},
	m_isShot{}
{
}

/// <summary>
/// デストラクタ
/// </summary>
Attack::~Attack()
{
}

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="tank">戦車情報</param>
void Attack::Initialize(Tank* tank)
{
	// 戦車情報の設定
	m_tank = tank;
	
	// 初期行動の設定
	m_currentAction = Action::SHOT;

	// 初期の移動時間の設定
	m_moveTime = mylib::Random(1.0f, MOVE_TIME);

	// 初期の攻撃時間の設定
	m_shotTime = mylib::Random(1.0f, SHOT_TIME);
}

/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">フレーム間の経過時間</param>
void Attack::Update(float elapsedTime)
{
	// 追跡対象の戦車がいないなら処理しない
	if (m_targetTank == nullptr) { return; }

	// 追跡対象の方向を向く
	this->LookTargetTank(elapsedTime);

	// 行動
	switch (m_currentAction)
	{
		case Attack::SHOT:
			this->ShotAction(elapsedTime);
			break;
		case Attack::MOVE:
			this->MoveAction(elapsedTime);
			break;
		default:
			break;
	}

	// 壁に当たっていたら壁の回避行動の遷移
	if (m_tank->GetAvoidWall())
	{
		Messenger::GetInstance()->Dispatch(m_tank->GetTankNumber(), Message::AVOIDWALL);
	}

	// 追跡対象の戦車が離れていたなら
	this->IsTargetTankFar();
}

/// <summary>
/// 行動状態遷移をした際に呼び出される関数
/// </summary>
void Attack::Enter()
{
	// 射撃情報のリセット
	m_isShot = false;

	// 初期の移動時間の設定
	m_moveTime = mylib::Random(1.0f, MOVE_TIME);

	// 初期の攻撃時間の設定
	m_shotTime = mylib::Random(1.0f, SHOT_TIME);
}

/// <summary>
/// 追跡対象の方向を向く
/// </summary>
/// <param name="elapsedTime">フレーム間の経過時間</param>
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
	targetAngle = mylib::Clamp(targetAngle, min, max);
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

/// <summary>
/// 移動処理
/// </summary>
/// <param name="elapsedTime">フレーム間の経過時間</param>
void Attack::MoveAction(float elapsedTime)
{
	using namespace DirectX::SimpleMath;

	// 時間経過
	m_moveTime -= elapsedTime;
	if (m_moveTime <= 0)
	{
		// 射撃行動に移行
		m_currentAction = Action::SHOT;
		// 移動時間を再設定
		m_moveTime = mylib::Random(1.0f, MOVE_TIME);
	}

	// 速度の設定
	float speed = Parameter::GetInstance()->GetEnemySpeed() * elapsedTime;
	Vector3 forward = Vector3::Forward * speed;
	Vector3 velocity = Vector3::Transform(forward, m_tank->GetRotation());
	
	// 移動処理
	m_tank->GetBody()->Move(velocity);
}


/// <summary>
/// 射撃処理
/// </summary>
/// <param name="elapsedTime">フレーム間の経過時間</param>
void Attack::ShotAction(float elapsedTime)
{
	m_shotTime -= elapsedTime;

	// 射撃行動の時間が終了したら攻撃行動に移行
	if (m_shotTime <= 0)
	{
		// 移動行動に移行
		m_currentAction = Action::MOVE;
		// 攻撃時間の再設定
		m_shotTime = mylib::Random(1.0f, SHOT_TIME);
	}

	// 射撃処理
	m_tank->GetCannon()->StartReload();
	m_tank->GetCannon()->Shoot();
	m_tank->GetCannon()->FinishShoot();

	// 射撃したフラグを立てる
	m_isShot = true;
}

/// <summary>
/// 追跡対象の戦車が離れている判定及び処理
/// </summary>
void Attack::IsTargetTankFar()
{
	// 追跡対象の戦車との距離を調べる
	float distance = (m_targetTank->GetPosition() - m_tank->GetPosition()).LengthSquared();

	// 追跡対象の戦車が離れているかつ一度でも射撃をしているなら追跡行動にする
	if (distance >= Parameter::GetInstance()->GetAttackFinishRadius() &&
		m_isShot)
	{
		// 追跡行動にする
		Messenger::GetInstance()->Dispatch(m_tank->GetTankNumber(), Message::TRACKING);
	}
}

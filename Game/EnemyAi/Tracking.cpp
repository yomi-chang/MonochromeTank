/*
	@file	Tracking.cpp
	@brief	敵の追跡行動クラス
*/
#include "pch.h"
#include "Game/EnemyAi/Tracking.h"

//-------------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------------
Tracking::Tracking()
	:
	m_targetTank{},
	m_tank{}
{
}

//-------------------------------------------------------------------
// 初期化処理
//-------------------------------------------------------------------
void Tracking::Initialize(Tank* tank)
{
	// 自機の取得
	m_tank = tank;
}

//-------------------------------------------------------------------
// 更新処理
//-------------------------------------------------------------------
void Tracking::Update(float elapsedTime)
{
	// 追跡対象の戦車がいないなら処理しない
	if (m_targetTank == nullptr) { return; }

	using namespace DirectX::SimpleMath;

	// 追跡中の敵の方向を向く
	// 敵の方向ベクトルの計算
	Vector3 delta = m_tank->GetPosition() - m_targetTank->GetPosition();
	float angleRadians = atan2(delta.x, delta.z);
	
	// 車体の回転を考慮して目標の角度を計算
	float targetAngle = angleRadians - m_tank->GetRotation().ToEuler().y;
	
	// 砲塔回転の制限
	targetAngle = mylib::Clamp(targetAngle, DirectX::XMConvertToRadians(-45.0f), DirectX::XMConvertToRadians(45.0f));
	
	// 現在の砲塔の回転角度
	float currentAngle = m_tank->GetTurret()->GetTurretRotation().ToEuler().y;
	
	// 目標角度と現在の角度との差を求め、Lerp補間で回転
	float angleDifference = targetAngle - currentAngle;
	
	// ゆっくり回転するための速度制御
	float rotationSpeed = 0.9f;
	float t = rotationSpeed * elapsedTime;
	
	// 補間後の回転角度
	float newAngle = currentAngle + angleDifference * t;
	
	// 砲塔の回転
	m_tank->GetTurret()->RotateTurret(newAngle);



	// 敵を追跡する
	// 進行方向ベクトル
	Vector3 heading = Vector3::Transform(Vector3::Forward * TANK_SPEED * elapsedTime, m_tank->GetRotation());

	// 自機からターゲットへ向かうベクトル
	Vector3 toTarget = m_targetTank->GetPosition() - m_tank->GetPosition();

	// 移動処理
	m_tank->GetBody()->Move(heading);

	/*
		自機をターゲットの方向へ徐々に回転する
	*/
	// 「自機の進行方向ベクトル」と「ターゲットの方向」からcosθを計算する
	float cosTheta = heading.Dot(toTarget) / (toTarget.Length() * heading.Length());

	// acosの引数で指定できる範囲は「-1～1」なので、値を補正する
	cosTheta = std::max(-1.0f, std::min(cosTheta, 1.0f));

	// cosθからθを計算する
	// acosの結果は「0～π」
	float theta = std::acos(cosTheta);

	//１フレームでの回転角を制限値以内に補正する
	theta = std::min(10.0f, theta);

	// 右側に行きたい場合は角度の符号を付け替える
	// ZX平面上にあるベクトルの向きはYのプラスマイナスで判断する
	if (heading.Cross(toTarget).y < 0.0f)
	{
		theta *= (-1.0f);
	}

	// 角度を更新する
	m_tank->GetBody()->Rotate(Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(theta), 0.0f, 0.0f));
}

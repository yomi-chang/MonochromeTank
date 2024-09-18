/*
	@file	FollowCamera.cpp
	@brief	後追いカメラクラス、ばね付き
*/
#include "pch.h"
#include "FollowCamera.h"
#include "Game/Screen.h"
#include "Game/Objects/NewTank/PlayerTank.h"
#include <cassert>

//-------------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------------
mylib::FollowCamera::FollowCamera()
	:
	m_eye{},
	m_target{},
	m_followUpTargetPosition{},
	m_followUpTargetQuaternion{},
	m_tank{},
	m_collider{},
	m_isShakeCamera{},
	m_shakeCount{},
	m_shakeSpeed{},
	m_shakeWidth{},
	m_shakeTime{}
{
}

//-------------------------------------------------------------------
// 初期化する
//-------------------------------------------------------------------

void mylib::FollowCamera::Initialize(PlayerTank* tank)
{
	// 自機情報の受け取り
	m_tank = tank;

	// コライダーの生成
	m_collider = std::make_unique<SphereCollider>();
	m_collider->CreateBoundingSphere(m_eye, 0.5f);

	// ビュー行列の作成のために一度Updateを呼ぶ
	this->Update(0.0f);
}

//-------------------------------------------------------------------
// 更新する
//-------------------------------------------------------------------
void mylib::FollowCamera::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	m_followUpTargetPosition = m_tank->GetPosition();
	//float tankAngleRL = m_tank->GetTankAngleRL();
	m_followUpTargetQuaternion = m_tank->GetAngle();

	// 基準になる「eye」を計算する
	DirectX::SimpleMath::Vector3 eye{ 0.0f, HEIGHT, DISTANCE };
	eye = DirectX::SimpleMath::Vector3::Transform(eye, m_followUpTargetQuaternion);

	// 基準になる「target」を計算する
	DirectX::SimpleMath::Vector3 target = m_followUpTargetPosition;

	// （ビュー行列で使用する）「m_eye」と「m_target」を計算する
	m_eye += ((target + eye) - m_eye) * SPRING_RATE_EYE;
	m_target += (target - m_target) * SPRING_RATE_TARGET;

	// コライダーの座標更新
	m_collider->Update(m_eye);

	// カメラの振動処理
	this->ShakeCamera(elapsedTime);
}

// カメラを揺らす処理
void mylib::FollowCamera::ShakeCamera(float elapsedTime)
{
	// カメラ振動がセットされていないなら早期リターン
	if (!m_isShakeCamera) { return; }

	m_shakeCount += elapsedTime;
	m_eye.z += std::sinf(m_shakeCount * m_shakeSpeed) * m_shakeWidth;

	if (m_shakeCount >= m_shakeTime)
	{
		m_isShakeCamera = false;
		m_shakeCount = 0.0f;
	}
}

// 振動開始
void mylib::FollowCamera::StartShakeCamera(
	float speed,
	float width,
	float time
)
{
	// 振動開始
	m_isShakeCamera = true;

	// 振動情報の設定
	m_shakeSpeed = speed;
	m_shakeWidth = width;
	m_shakeTime = time;
}


/*
	@file	FollowCamera.cpp
	@brief	後追いカメラクラス、ばね付き
*/
#include "pch.h"
#include "FollowCamera.h"
#include "Game/Screen.h"
#include "Game/Objects/Tank/Tank.h"
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
	m_collider{}
{
}

//-------------------------------------------------------------------
// 初期化する
//-------------------------------------------------------------------

void mylib::FollowCamera::Initialize(Tank* tank)
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

	m_followUpTargetPosition = m_tank->GetTankPosition();
	float tankAngleRL = m_tank->GetTankAngleRL();
	m_followUpTargetQuaternion = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll({ 0.0f,tankAngleRL,0.0f });

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
}

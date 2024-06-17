/*
	@file	FollowCamera.cpp
	@brief	後追いカメラクラス、ばね付き
*/
#include "pch.h"
#include "FollowCamera.h"
#include "Game/Screen.h"
#include <cassert>

//-------------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------------
mylib::FollowCamera::FollowCamera()
	:
	m_eye{},
	m_target{},
	m_followUpTargetPosition{},
	m_followUpTargetQuaternion{}
{
}

//-------------------------------------------------------------------
// 初期化する
//-------------------------------------------------------------------
void mylib::FollowCamera::Initialize(
	const DirectX::SimpleMath::Vector3* followUpTargetPosition,
	const DirectX::SimpleMath::Quaternion* followUpTargetQuaternion
)
{
	assert(followUpTargetPosition);
	assert(followUpTargetQuaternion);

	// ターゲットに関するポインタを保持する
	m_followUpTargetPosition = followUpTargetPosition;
	m_followUpTargetQuaternion = followUpTargetQuaternion;
}

//-------------------------------------------------------------------
// 更新する
//-------------------------------------------------------------------
void mylib::FollowCamera::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// 基準になる「eye」を計算する
	DirectX::SimpleMath::Vector3 eye{ 0.0f, HEIGHT, DISTANCE };
	eye = DirectX::SimpleMath::Vector3::Transform(eye, *m_followUpTargetQuaternion);

	// 基準になる「target」を計算する
	DirectX::SimpleMath::Vector3 target = *m_followUpTargetPosition;

	// （ビュー行列で使用する）「m_eye」と「m_target」を計算する
	m_eye += ((target + eye) - m_eye) * SPRING_RATE_EYE;
	m_target += (target - m_target) * SPRING_RATE_TARGET;
}

/*
	@file	LockOnCamera.cpp
	@brief	移動可能なカメラクラス
*/
#include "pch.h"
#include "Libraries/MyLib/LockOnCamera.h"

//-------------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------------
mylib::LockOnCamera::LockOnCamera()
	:
	m_eye{},
	m_target{},
	m_targetPosition{},
	m_followUpTargetQuaternion{},
	m_distance{},
	m_height{}
{
}

//-------------------------------------------------------------------
// 初期化する
//-------------------------------------------------------------------

void mylib::LockOnCamera::Initialize()
{

	// ビュー行列の作成のために一度Updateを呼ぶ
	this->Update(0.0f);

	m_distance = DISTANCE;
	m_height = HEIGHT;
}

//-------------------------------------------------------------------
// 更新する
//-------------------------------------------------------------------
void mylib::LockOnCamera::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);
	using namespace DirectX::SimpleMath;

	// 戦車の座標の角度の情報を受け取る
	//m_followUpTargetQuaternion = m_tank->GetRotation();

	// 基準になる「eye」を計算する
	DirectX::SimpleMath::Vector3 eye{ 0.0f, m_height, m_distance };

	// 基準になる「target」を計算する
	DirectX::SimpleMath::Vector3 target = m_targetPosition;

	// （ビュー行列で使用する）「m_eye」と「m_target」を計算する
	m_eye += ((target + eye) - m_eye) * SPRING_RATE_EYE;
	m_target += (target - m_target) * SPRING_RATE_TARGET;
}
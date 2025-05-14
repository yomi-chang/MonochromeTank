/*
	@file	AvoidWall.cpp
	@brief	敵の壁回避クラス
*/
#include "pch.h"
#include "Game/EnemyAi/AvoidWall.h"
#include "Libraries/MyLib/Utils.h"

//-------------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------------
AvoidWall::AvoidWall()
	:
	m_tank{},
	m_targetTank{},
	m_time{}
{
}

//-------------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------------
AvoidWall::~AvoidWall()
{
}

//-------------------------------------------------------------------
// 初期化処理
//-------------------------------------------------------------------
void AvoidWall::Initialize(Tank* tank)
{
	m_tank = tank;
}

//-------------------------------------------------------------------
// 更新処理
//-------------------------------------------------------------------
void AvoidWall::Update(float elapsedTime)
{
	using namespace DirectX::SimpleMath;

	// 回避行動
	auto parameter = Parameter::GetInstance();
	float speed = elapsedTime * parameter->GetEnemySpeed();
	float angle = DirectX::XMConvertToRadians(0.7f);
	Vector3 velocity = Vector3::Transform(Vector3::Forward * speed, m_tank->GetRotation());
	// 移動させる
	m_tank->GetBody()->Move(velocity);
	// 回転させる
	m_tank->GetBody()->Rotate(Quaternion::CreateFromYawPitchRoll(angle, 0.0f, 0.0f));

	// 壁を回避出来たなら前回の行動に遷移する
	if (!m_tank->GetAvoidWall())
	{
 		Messenger::GetInstance()->Dispatch(m_tank->GetTankNumber(), Message::NONE);
	}
}

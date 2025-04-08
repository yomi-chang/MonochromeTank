/*
	@file	Attack.cpp
	@brief	“G‚ÌUŒ‚ˆ—ƒNƒ‰ƒX
*/
#include "pch.h"
#include "Game/EnemyAi/Attack.h"
#include "Libraries/MyLib/Math.h"

//-------------------------------------------------------------------
// ƒRƒ“ƒXƒgƒ‰ƒNƒ^
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
// ƒRƒ“ƒXƒgƒ‰ƒNƒ^
//-------------------------------------------------------------------
Attack::~Attack()
{
}

//-------------------------------------------------------------------
// ‰Šú‰»ˆ—
//-------------------------------------------------------------------
void Attack::Initialize(Tank* tank)
{
	m_tank = tank;
	m_currentAction = Action::SHOT;

	// ‰Šú‚ÌˆÚ“®ŽžŠÔ‚ÌÝ’è
	m_moveTime = mylib::Random(1.0f, MOVE_TIME);

	// ‰Šú‚ÌUŒ‚ŽžŠÔ‚ÌÝ’è
	m_shotTime = mylib::Random(1.0f, SHOT_TIME);
}

//-------------------------------------------------------------------
// XVˆ—
//-------------------------------------------------------------------
void Attack::Update(float elapsedTime)
{
	// ’ÇÕ‘ÎÛ‚ÌíŽÔ‚ª‚¢‚È‚¢‚È‚çˆ—‚µ‚È‚¢
	if (m_targetTank == nullptr) { return; }

	// ’ÇÕ‘ÎÛ‚Ì•ûŒü‚ðŒü‚­
	LookTargetTank(elapsedTime);

	// ŽËŒ‚ˆ—
	m_tank->GetCannon()->StartReload();
	m_tank->GetCannon()->Shoot();
	m_tank->GetCannon()->FinishShoot();

	// s“®
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

	// ’ÇÕ‘ÎÛ‚ÌíŽÔ‚ª—£‚ê‚Ä‚¢‚½‚È‚ç
	this->IsTargetTankFar();
}

//-------------------------------------------------------------------
// ’ÇÕ‘ÎÛ‚Ì•ûŒü‚ðŒü‚­
//-------------------------------------------------------------------
void Attack::LookTargetTank(float elapsedTime)
{
	using namespace DirectX::SimpleMath;

	// ƒpƒ‰ƒ[ƒ^‚ÌŽó‚¯Žæ‚è
	const auto& parameter = Parameter::GetInstance();

	// “G‚Ì•ûŒüƒxƒNƒgƒ‹‚ÌŒvŽZ
	Vector3 delta = m_tank->GetPosition() - m_targetTank->GetPosition();
	float angleRadians = atan2(delta.x, delta.z);

	// ŽÔ‘Ì‚Ì‰ñ“]‚ðl—¶‚µ‚Ä–Ú•W‚ÌŠp“x‚ðŒvŽZ
	float targetAngle = angleRadians - m_tank->GetRotation().ToEuler().y;

	// –C“ƒ‰ñ“]‚Ì§ŒÀ
	float min = parameter->GetTurretAngleMin();
	float max = parameter->GetTurretAngleMax();
	targetAngle = mylib::Clamp(targetAngle, min, max);

	// Œ»Ý‚Ì–C“ƒ‚Ì‰ñ“]Šp“x
	float currentAngle = m_tank->GetTurret()->GetTurretRotation().ToEuler().y;

	// –Ú•WŠp“x‚ÆŒ»Ý‚ÌŠp“x‚Æ‚Ì·‚ð‹‚ßALerp•âŠÔ‚Å‰ñ“]
	float angleDifference = targetAngle - currentAngle;

	// ‚ä‚Á‚­‚è‰ñ“]‚·‚é‚½‚ß‚Ì‘¬“x§Œä
	float rotationSpeed = parameter->GetEnemyRotationSpeed() * elapsedTime;

	// •âŠÔŒã‚Ì‰ñ“]Šp“x
	float newAngle = currentAngle + angleDifference * rotationSpeed;

	// –C“ƒ‚Ì‰ñ“]
	m_tank->GetTurret()->RotateTurret(newAngle);
}

//-------------------------------------------------------------------
// ˆÚ“®
//-------------------------------------------------------------------
void Attack::MoveAction(float elapsedTime)
{
	using namespace DirectX::SimpleMath;
	auto parameter = Parameter::GetInstance();
	float speed = parameter->GetEnemySpeed() * elapsedTime;

	// ŽžŠÔŒo‰ß
	m_time += elapsedTime;
	if (m_time >= m_moveTime)
	{
		m_time = 0.0f;
		m_currentAction = Action::SHOT;
		// ˆÚ“®ŽžŠÔ‚ðÄ“xÝ’è
		m_moveTime = mylib::Random(1.0f, MOVE_TIME);
	}

	// ˆÚ“®
	Vector3 velocity = Vector3::Transform(Vector3::Forward * speed, m_tank->GetRotation());

	// ˆÚ“®ˆ—
	m_tank->GetBody()->Move(velocity);
}

//-------------------------------------------------------------------
// ŽËŒ‚
//-------------------------------------------------------------------
void Attack::ShotAction(float elapsedTime)
{
	m_time += elapsedTime;
	if (m_time >= m_shotTime)
	{
		m_time = 0.0f;
		m_currentAction = Action::MOVE;
		// ‰Šú‚ÌUŒ‚ŽžŠÔ‚ÌÝ’è
		m_shotTime = mylib::Random(1.0f, SHOT_TIME);
	}

	// ŽËŒ‚ˆ—
	m_tank->GetCannon()->StartReload();
	m_tank->GetCannon()->Shoot();
}

//-------------------------------------------------------------------
// ’ÇÕ‘ÎÛ‚ÌíŽÔ‚ª—£‚ê‚Ä‚¢‚é
//-------------------------------------------------------------------
void Attack::IsTargetTankFar()
{
	// ’ÇÕ‘ÎÛ‚ÌíŽÔ‚Æ‚Ì‹——£‚ð’²‚×‚é
	float distance = (m_targetTank->GetPosition() - m_tank->GetPosition()).LengthSquared();

	// ’ÇÕ‘ÎÛ‚ÌíŽÔ‚ª—£‚ê‚Ä‚¢‚é‚È‚ç’ÇÕs“®‚É‚·‚é
	if (distance >= Parameter::GetInstance()->GetAttackFinishRadius())
	{
		// ’ÇÕs“®‚É‚·‚é
		Messenger::GetInstance()->Dispatch(m_tank->GetTankNumber(), Message::TRACKING);
	}
}

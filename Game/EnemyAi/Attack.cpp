#include "pch.h"
#include "Game/EnemyAi/Attack.h"

//-------------------------------------------------------------------
// ƒRƒ“ƒXƒgƒ‰ƒNƒ^
//-------------------------------------------------------------------
Attack::Attack()
	:
	m_tank{},
	m_targetTank{},
	m_time{}
{
}
//-------------------------------------------------------------------
// ‰Šú‰»ˆ—
//-------------------------------------------------------------------
void Attack::Initialize(Tank* tank)
{
	m_tank = tank;
	m_currentAction = Action::SHOT;
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
}

// ’ÇÕ‘ÎÛ‚Ì•ûŒü‚ðŒü‚­
void Attack::LookTargetTank(float elapsedTime)
{
	using namespace DirectX::SimpleMath;



	// “G‚Ì•ûŒüƒxƒNƒgƒ‹‚ÌŒvŽZ
	Vector3 delta = m_tank->GetPosition() - m_targetTank->GetPosition();
	float angleRadians = atan2(delta.x, delta.z);

	// ŽÔ‘Ì‚Ì‰ñ“]‚ðl—¶‚µ‚Ä–Ú•W‚ÌŠp“x‚ðŒvŽZ
	float targetAngle = angleRadians - m_tank->GetRotation().ToEuler().y;

	// –C“ƒ‰ñ“]‚Ì§ŒÀ
	targetAngle = mylib::Clamp(targetAngle, DirectX::XMConvertToRadians(-45.0f), DirectX::XMConvertToRadians(45.0f));

	// Œ»Ý‚Ì–C“ƒ‚Ì‰ñ“]Šp“x
	float currentAngle = m_tank->GetTurret()->GetTurretRotation().ToEuler().y;

	// –Ú•WŠp“x‚ÆŒ»Ý‚ÌŠp“x‚Æ‚Ì·‚ð‹‚ßALerp•âŠÔ‚Å‰ñ“]
	float angleDifference = targetAngle - currentAngle;

	// ‚ä‚Á‚­‚è‰ñ“]‚·‚é‚½‚ß‚Ì‘¬“x§Œä
	float rotationSpeed = 1.0f;
	float t = rotationSpeed * elapsedTime;

	// •âŠÔŒã‚Ì‰ñ“]Šp“x
	float newAngle = currentAngle + angleDifference * t;

	// –C“ƒ‚Ì‰ñ“]
	m_tank->GetTurret()->RotateTurret(newAngle);
}

void Attack::MoveAction(float elapsedTime)
{
	using namespace DirectX::SimpleMath;

	// ŽžŠÔŒo‰ß
	m_time += elapsedTime;
	if (m_time >= MOVE_TIME)
	{
		m_time = 0.0f;
		m_currentAction = Action::SHOT;
	}

	// ˆÚ“®
	Vector3 moveVector = Vector3::Transform(Vector3::Forward, m_tank->GetRotation());

	moveVector.Normalize();
	moveVector *= TANK_SPEED * elapsedTime;

	// ˆÚ“®ˆ—
	m_tank->GetBody()->Move(moveVector);
}

void Attack::ShotAction(float elapsedTime)
{
	m_time += elapsedTime;
	if (m_time >= SHOT_TIME)
	{
		m_time = 0.0f;
		m_currentAction = Action::MOVE;
	}

	// ŽËŒ‚ˆ—
	m_tank->GetCannon()->StartReload();
	m_tank->GetCannon()->Shoot();
}

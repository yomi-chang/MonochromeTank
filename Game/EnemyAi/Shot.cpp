/*
	@file	Shot.cpp
	@brief	“G‚ÌŽËŒ‚ˆ—ƒNƒ‰ƒX
*/
#include "pch.h"
#include "Shot.h"

//-------------------------------------------------------------------
// ƒRƒ“ƒXƒgƒ‰ƒNƒ^
//-------------------------------------------------------------------
Shot::Shot()
	:
	m_tank{},
	m_targetTank{}
{
}

//-------------------------------------------------------------------
// ƒfƒXƒgƒ‰ƒNƒ^
//-------------------------------------------------------------------
Shot::~Shot()
{
}

//-------------------------------------------------------------------
// ‰Šú‰»ˆ—
//-------------------------------------------------------------------
void Shot::Initialize(Tank* tank)
{
	// Ž©‹@‚ÌŽæ“¾
	m_tank = tank;
}

//-------------------------------------------------------------------
// XVˆ—
//-------------------------------------------------------------------
void Shot::Update(float elapsedTime)
{
	// ’ÇÕ‘ÎÛ‚ÌíŽÔ‚ª‚¢‚È‚¢‚È‚çˆ—‚µ‚È‚¢
	if (m_targetTank == nullptr) { return; }

	// “G‚Ì•ûŒü‚ðí‚ÉŒü‚­
	this->LookTargetTank(elapsedTime);

	// ŽËŒ‚ˆ—
	m_tank->GetCannon()->StartReload();
	m_tank->GetCannon()->Shoot();
	m_tank->GetCannon()->FinishShoot();
}

//-------------------------------------------------------------------
// ’ÇÕ‘ÎÛ‚Ì•ûŒü‚ðŒü‚­
//-------------------------------------------------------------------
void Shot::LookTargetTank(float elapsedTime)
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

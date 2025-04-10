/*
	@file	CollisionManager.cpp
	@brief	“–‚½‚è”»’èŠÇ—ƒNƒ‰ƒX
*/
#include "pch.h"
#include "Game/Other/CollisionManager.h"
#include "Game/Collider/BoxCollider.h"
#include "Game/Collider/SphereCollider.h"
#include "Game/Objects/Tank/TankBase/Tank.h"
#include "Game/Objects/Stage/Wall.h"
#include "Game/Objects/FixedTurret/FixedTurret.h"
#include "Game/Other/Parameter.h"

//-------------------------------------------------------------------
// ƒRƒ“ƒXƒgƒ‰ƒNƒ^
//-------------------------------------------------------------------
CollisionManager::CollisionManager()
	:
	m_tanks{},
	m_camera{},
	m_fixedTurrets{},
	m_walls{},
	m_wallGimmick{}
{
}

//-------------------------------------------------------------------
// ƒfƒXƒgƒ‰ƒNƒ^
//-------------------------------------------------------------------
CollisionManager::~CollisionManager()
{
}

//-------------------------------------------------------------------
// XVˆ—
//-------------------------------------------------------------------
void CollisionManager::Update()
{
	// íŽÔ‚Ì–C’e‚Ì“–‚½‚è”»’è
	DetectCollisionTankAndCannonBall();

	// íŽÔ‚Æ˜AŽË’e‚Ì“–‚½‚è”»’è
	DetectCollisionTankAndNomalBullets();

	// íŽÔ“¯Žm‚Ì“–‚½‚è”»’è
	DetectCollisionTankAndOtherTanks();

	// íŽÔ‚Æ•Ç‚Ì“–‚½‚è”»’è
	DetectCollisionTankAndWalls();

	// ’e‚Æ•Ç‚Ì“–‚½‚è”»’è
	DetectCollisionBulletsAndWalls();

	// Ray‚Æ•Ç‚Ì“–‚½‚è”»’è
	DetectCollisionRayAndWalls();

	// •Ç‚Æ•ÇŒŸ’m—pƒRƒ‰ƒCƒ_[‚Ì“–‚½‚è”»’è
	DetectCollisionWallAndAvoidCollider();
}

// íŽÔ‚Æ˜AŽË’e
void CollisionManager::DetectCollisionTankAndNomalBullets()
{
	for (auto& tank : m_tanks)
	{
		// ”j‰ó‚³‚ê‚Ä‚¢‚éê‡
		if (tank->GetDead()) { continue; }

		// ‘¼‚ÌíŽÔ‚Ì’e
		for (auto& otherTank : m_tanks)
		{
			// Ž©‹@‚Ìê‡‚©‘ŠŽè‚ª”j‰ó‚³‚ê‚Ä‚¢‚éê‡
			if (tank->GetTankNumber() == otherTank->GetTankNumber() ||
				otherTank->GetDead())
			{
				continue; 
			}

			for (auto& bullet : otherTank->GetCannon()->GetBullets())
			{
				// ’eŠÛ‚ª”ò‚ñ‚Å‚¢‚éA‚©‚Â“–‚½‚Á‚Ä‚¢‚é‚È‚ç
				if (bullet->GetBulletState() == IBullet::FLYING &&
					tank->GetCollider()->CheckTriggerCollider(bullet->GetBoundingSphere()))
				{
					// ’e‚ðŽg—pÏ‚Ý‚É‚·‚é
					bullet->SetBulletState(IBullet::USED);

					// ƒ_ƒ[ƒWˆ—
					tank->Damage(Parameter::GetInstance()->GetBulletDamage());

					// UŒ‚‚µ‚Ä‚«‚½íŽÔ‚Ìî•ñ‚ð‹L‰¯‚·‚é
					tank->SetTargetTank(otherTank);
				}
			}
		}

		// ŒÅ’è–C‘ä‚Ì’e
		for (auto& fixedTurret : m_fixedTurrets)
		{
			for (auto& bullet : fixedTurret->GetBullets())
			{
				// ’eŠÛ‚ª”ò‚ñ‚Å‚¢‚éA‚©‚Â“–‚½‚Á‚Ä‚¢‚é‚È‚ç
				if (bullet->GetBulletState() == IBullet::FLYING &&
					tank->GetCollider()->CheckTriggerCollider(bullet->GetBoundingSphere()))
				{
					// ’e‚ðŽg—pÏ‚Ý‚É‚·‚é
					bullet->SetBulletState(IBullet::USED);

					// ƒ_ƒ[ƒWˆ—
					tank->Damage(Parameter::GetInstance()->GetBulletDamage());
				}
			}
		}
	}
}

// íŽÔ‚Æ–C’e
void CollisionManager::DetectCollisionTankAndCannonBall()
{
	for (auto& tank : m_tanks)
	{
		// ”j‰ó‚³‚ê‚Ä‚¢‚éê‡
		if (tank->GetDead()) { continue; }

		for (auto& otherTank : m_tanks)
		{
			// Ž©‹@‚Ìê‡‚©‘ŠŽè‚ª”j‰ó‚³‚ê‚Ä‚¢‚éê‡
			if (tank->GetTankNumber() == otherTank->GetTankNumber() ||
				otherTank->GetDead()) 
			{
				continue;
			}

			// ‘¼‚ÌíŽÔ‚Ì’e‚ª“–‚½‚Á‚Ä‚¢‚½‚çƒ_ƒ[ƒW
			if (otherTank->GetCannon()->GetCannonBall()->GetBulletState() == IBullet::FLYING &&
				tank->GetCollider()->CheckTriggerCollider(otherTank->GetCannon()->GetCannonBall()->GetBoundingSphere()))
			{
				// ’e‚ðŽg—pÏ‚Ý‚É‚·‚é
				otherTank->GetCannon()->GetCannonBall()->SetBulletState(IBullet::USED);

				// ƒ_ƒ[ƒWˆ—
				tank->Damage(Parameter::GetInstance()->GetCannonBallDamage());

				// UŒ‚‚µ‚Ä‚«‚½íŽÔ‚Ìî•ñ‚ð‹L‰¯‚·‚é
				tank->SetTargetTank(otherTank);
			}
		}
	}
}

// íŽÔ“¯Žm
void CollisionManager::DetectCollisionTankAndOtherTanks()
{
	for (auto& tank : m_tanks)
	{
		// ”j‰ó‚³‚ê‚Ä‚¢‚éê‡
		if (tank->GetHp() <= 0) { continue; }

		for (auto& otherTank : m_tanks)
		{
			// Ž©‹@‚Ìê‡‚©‘ŠŽè‚ª”j‰ó‚³‚ê‚Ä‚¢‚éê‡
			if (tank->GetTankNumber() == otherTank->GetTankNumber() ||
				otherTank->GetHp() <= 0)
			{
				continue;
			}

			// ‰Ÿ‚µ–ß‚µˆ—
			DirectX::SimpleMath::Vector3 collisionVel = tank->GetCollider()->CheckCollisionCollider(otherTank->GetBoundingBox());
			collisionVel.y = 0.0f;
			otherTank->GetBody()->SetCollisionVel(collisionVel);
		}
	}
}

// íŽÔ‚Æ•Ç
void CollisionManager::DetectCollisionTankAndWalls()
{
	for (auto& wall : m_walls)
	{
		for (auto& tank : m_tanks)
		{
			// ”j‰ó‚³‚ê‚Ä‚¢‚éê‡
			if (tank->GetDead()) { continue; }

			// •Ç‚ÌƒRƒ‰ƒCƒ_[Žó‚¯Žæ‚è
			BoxCollider* wallCollider = wall->GetCollider();

			// ‰Ÿ‚µ–ß‚µˆ—
			DirectX::SimpleMath::Vector3 collisionVel = wallCollider->CheckCollisionCollider(tank->GetBoundingBox());
			collisionVel.y = 0.0f;
			tank->GetBody()->SetCollisionVel(collisionVel);
			
			// •Ç‚ÉG‚ê‚Ä‚¢‚éŠÔd—Í‚ð–³Œø‚É‚·‚é
			if (wallCollider->CheckTriggerCollider(tank->GetBoundingBox()))
				tank->GetBody()->SetGravity(false);
		}
	}
}

// ’e‚Æ•Ç
void CollisionManager::DetectCollisionBulletsAndWalls()
{
	for (auto& tank : m_tanks)
	{
		// ”j‰ó‚³‚ê‚Ä‚¢‚éê‡
		if (tank->GetDead()) { continue; }

		for (auto& wall : m_walls)
		{
			// •Ç‚ÌƒRƒ‰ƒCƒ_[Žó‚¯Žæ‚è
			BoxCollider* wallCollider = wall->GetCollider();

			// –C’e‚Æ•Ç‚Ì”»’èA“–‚½‚Á‚Ä‚¢‚½‚çŽg—pÏ‚Ý‚É
			if (wallCollider->CheckTriggerCollider(tank->GetCannon()->GetCannonBall()->GetBoundingSphere()) &&
				tank->GetCannon()->GetCannonBall()->GetBulletState() == IBullet::FLYING)
			{
				tank->GetCannon()->GetCannonBall()->SetBulletState(IBullet::USED);
			}
			// ˜AŽË’e‚Æ•Ç‚Ì”»’èA“–‚½‚Á‚Ä‚¢‚½‚çŽg—pÏ‚Ý‚É
			for (auto& bullet : tank->GetCannon()->GetBullets())
			{
				if (wallCollider->CheckTriggerCollider(bullet->GetBoundingSphere()) &&
					bullet->GetBulletState() == IBullet::FLYING)
				{
					bullet->SetBulletState(IBullet::USED);
				}
			}
		}
	}
}

// Ray‚Æ•Ç
void CollisionManager::DetectCollisionRayAndWalls()
{
	using namespace DirectX::SimpleMath;

	Vector3 rayDirection;
	float rayDistance = 0.0f;
	float hitDistance = 0.0f;
	bool isHit = false;

	for (auto& tank : m_tanks)
	{
		// ”j‰ó‚³‚ê‚Ä‚¢‚éê‡
		if (tank->GetDead()) { continue; }

		// ƒvƒŒƒCƒ„[‚Ì‚Ý”»’è‚ðs‚¤
		if (tank->GetTankNumber() == 0) 
		{
			// Ray‚Ìì¬
			Quaternion rotation = tank->GetCannon()->GetMuzzleRotation();
			Matrix matrix = Matrix::CreateFromQuaternion(rotation);
			rayDirection = matrix.Forward();
			rayDirection.Normalize();
			Ray ray{ tank->GetCannon()->GetMuzzlePosition(),rayDirection };
			// ŽË“I‹——£‚ÌŽæ“¾
			rayDistance = tank->GetCannon()->GetMaxRange();
			float minDistance = rayDistance;

			// •Ç‚ÆRay‚ÌÕ“Ë
			for (auto& wall : m_walls)
			{
				isHit = ray.Intersects(*wall->GetBoundingBox(), hitDistance);

				if (isHit && hitDistance <= rayDistance)
				{
					// ‚æ‚è’Z‚¢‹——£‚ð‘ã“ü‚·‚é
					minDistance = std::min(minDistance, hitDistance);
				}
			}
			// “G‚ÆRay‚ÌÕ“Ë
			for (auto& otherTank : m_tanks)
			{
				// Ž©‹@‚Ìê‡‚©‘ŠŽè‚ª”j‰ó‚³‚ê‚Ä‚¢‚éê‡
				if (tank->GetTankNumber() == otherTank->GetTankNumber() ||
					otherTank->GetDead())
				{
					continue;
				}

				isHit = ray.Intersects(*otherTank->GetBoundingBox(), hitDistance);
				if (isHit && hitDistance <= rayDistance)
				{
					// ‚æ‚è’Z‚¢‹——£‚ð‘ã“ü‚·‚é
					minDistance = std::min(minDistance, hitDistance);
				}
			}

			// Õ“ËÀ•W
			Vector3 hitPosition = Vector3::Zero;
			// ˆê‰ñ‚Å‚à•Ç‚Æ‚ÌÕ“Ë‚ªŽæ‚ê‚Ä‚¢‚½‚ç
			if (minDistance < rayDistance)
			{
				// Õ“Ë“_ŒvŽZ
				hitPosition = Vector3{ ray.position + ray.direction * minDistance - ray.direction };
				isHit = true;
			}
			else
			{
				// Æ€‰æ‘œ‚Ì•\Ž¦êŠŒvŽZ
				hitPosition = Vector3{ ray.position + ray.direction * rayDistance - ray.direction };

				// °‚É–„‚Ü‚ç‚È‚¢‚æ‚¤‚É‚·‚é
				if (hitPosition.y <= 0.0f)
				{
					hitPosition.y = 0.0f;
					isHit = true;
				}
			}
			// Ray‚ÌÕ“Ëî•ñ‚ÌÝ’è
			tank->GetCannon()->SetRayInfo(isHit, hitPosition);
		}
	}
}

// •Ç‚Æ‰ñ”ð—pƒRƒ‰ƒCƒ_[
void CollisionManager::DetectCollisionWallAndAvoidCollider()
{
	using namespace DirectX::SimpleMath;

	for (auto& tank : m_tanks)
	{
		// ƒvƒŒƒCƒ„[‚Í”»’è‚ðs‚í‚È‚¢
		if (tank->GetTankNumber() == 0 ||
			tank->GetDead())
		{ 
			continue; 
		}

		for (auto& wall : m_walls)
		{
			if(tank->GetAvoidCollider()->CheckTriggerCollider(wall->GetBoundingBox()))
			{
				tank->SetAvoidWall(true);
				return;
			}
			else
			{
				tank->SetAvoidWall(false);
			}
		}
	}
}



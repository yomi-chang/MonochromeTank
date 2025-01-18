#include "pch.h"
#include "Game/Other/CollisionManager.h"
#include "Game/Collider/BoxCollider.h"
#include "Game/Collider/SphereCollider.h"
#include "Game/Objects/Tank/TankBase/Tank.h"
#include "Game/Objects/Stage/Wall.h"

CollisionManager::CollisionManager()
{
}

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
}

// íŽÔ‚Æ˜AŽË’e‚Ì“–‚½‚è”»’è
void CollisionManager::DetectCollisionTankAndNomalBullets()
{
	for (auto& tank : m_tanks)
	{
		for (auto& otherTank : m_tanks)
		{
			// Ž©‹@‚Ìê‡‚Í”»’è‚ðs‚í‚È‚¢
			if (tank->GetTankNumber() == otherTank->GetTankNumber()) { continue; }

			for (auto& bullet : otherTank->GetCannon()->GetBullets())
			{
				// ’eŠÛ‚ª”ò‚ñ‚Å‚¢‚éA‚©‚Â“–‚½‚Á‚Ä‚¢‚é‚È‚ç
				if (bullet->GetBulletState() == IBullet::FLYING &&
					tank->GetCollider()->CheckTriggerCollider(bullet->GetBoundingSphere()))
				{
					bullet->SetBulletState(IBullet::USED);
					tank->Damage(1);
				}
			}
		}
	}
}

// íŽÔ‚Æ–C’e‚Ì“–‚½‚è”»’è
void CollisionManager::DetectCollisionTankAndCannonBall()
{
	for (auto& tank : m_tanks)
	{
		for (auto& otherTank : m_tanks)
		{
			// Ž©‹@‚Ìê‡‚Í”»’è‚ðs‚í‚È‚¢
			if (tank->GetTankNumber() == otherTank->GetTankNumber()) { continue; }

			// ‘¼‚ÌíŽÔ‚Ì’e‚ª“–‚½‚Á‚Ä‚¢‚½‚çƒ_ƒ[ƒW
			if (otherTank->GetCannon()->GetCannonBall()->GetBulletState() == IBullet::FLYING &&
				tank->GetCollider()->CheckTriggerCollider(otherTank->GetCannon()->GetCannonBall()->GetBoundingSphere()))
			{
				otherTank->GetCannon()->GetCannonBall()->SetBulletState(IBullet::USED);
				tank->Damage(10);
			}
		}
	}
}

// íŽÔ“¯Žm‚Ì“–‚½‚è”»’è
void CollisionManager::DetectCollisionTankAndOtherTanks()
{
	for (auto& tank1 : m_tanks)
	{
		for (auto& tank2 : m_tanks)
		{
			// Ž©‹@‚Ìê‡‚Í”»’è‚ðs‚í‚È‚¢
			if (tank1->GetTankNumber() == tank2->GetTankNumber()) { continue; }

			tank2->GetBody()->SetCollisionVel(tank1->GetCollider()->CheckCollisionCollider(tank2->GetBoundingBox()));
		}
	}
}

// íŽÔ‚Æ•Ç‚Ì“–‚½‚è”»’è
void CollisionManager::DetectCollisionTankAndWalls()
{
	for (auto& wall : m_walls)
	{
		for (auto& tank : m_tanks)
		{
			// •Ç‚ÌƒRƒ‰ƒCƒ_[Žó‚¯Žæ‚è
			BoxCollider* wallCollider = wall->GetCollider();

			tank->GetBody()->SetCollisionVel(wallCollider->CheckCollisionCollider(tank->GetBoundingBox()));
			// •Ç‚ÉG‚ê‚Ä‚¢‚éŠÔd—Í‚ð–³Œø‚É‚·‚é
			if (wallCollider->CheckTriggerCollider(tank->GetBoundingBox()))
				tank->GetBody()->SetGravity(false);
		}
	}
}

// ’e‚Æ•Ç‚Ì“–‚½‚è”»’è
void CollisionManager::DetectCollisionBulletsAndWalls()
{
	for (auto& tank : m_tanks)
	{
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



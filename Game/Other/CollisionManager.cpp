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

	// Ray‚Æ•Ç‚Ì“–‚½‚è”»’è
	DetectCollisionRayAndWalls();
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
					// ’e‚ðŽg—pÏ‚Ý‚É‚·‚é
					bullet->SetBulletState(IBullet::USED);

					// ƒ_ƒ[ƒWˆ—
					tank->Damage(1);

					// UŒ‚‚µ‚Ä‚«‚½íŽÔ‚Ìî•ñ‚ð‹L‰¯‚·‚é
					tank->SetTargetTank(otherTank);
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
				// ’e‚ðŽg—pÏ‚Ý‚É‚·‚é
				otherTank->GetCannon()->GetCannonBall()->SetBulletState(IBullet::USED);

				// ƒ_ƒ[ƒWˆ—
				tank->Damage(10);

				// UŒ‚‚µ‚Ä‚«‚½íŽÔ‚Ìî•ñ‚ð‹L‰¯‚·‚é
				tank->SetTargetTank(otherTank);
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

			DirectX::SimpleMath::Vector3 collisionVel = tank1->GetCollider()->CheckCollisionCollider(tank2->GetBoundingBox());
			collisionVel.y = 0.0f;
			tank2->GetBody()->SetCollisionVel(collisionVel);
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

			DirectX::SimpleMath::Vector3 collisionVel = wallCollider->CheckCollisionCollider(tank->GetBoundingBox());
			collisionVel.y = 0.0f;
			tank->GetBody()->SetCollisionVel(collisionVel);
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

// Ray‚Æ•Ç‚Ì“–‚½‚è”»’è
void CollisionManager::DetectCollisionRayAndWalls()
{
	using namespace DirectX::SimpleMath;

	Vector3 rayDirection;
	float rayDistance = 4.0f;
	float hitDistance = 0.0f;
	bool isHit = false;

	for (auto& tank : m_tanks)
	{
		// ƒvƒŒƒCƒ„[‚Í”»’è‚ðs‚í‚È‚¢
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
				// Ž©‹@‚Ìê‡‚Í”»’è‚ðs‚í‚È‚¢
				if (tank->GetTankNumber() == otherTank->GetTankNumber()) { continue; }
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
		// ‚»‚êˆÈŠO
		else
		{
			rayDirection = Matrix::CreateFromQuaternion(tank->GetRotation()).Forward();
			rayDirection.Normalize();
			Ray ray{ tank->GetPosition(),rayDirection };

			for (auto& wall : m_walls)
			{
				isHit = ray.Intersects(*wall->GetBoundingBox(), hitDistance);

				if (isHit && hitDistance <= rayDistance)
				{
					float angle = DirectX::XMConvertToRadians(1.7f);
					tank->GetBody()->Rotate(Quaternion::CreateFromYawPitchRoll(angle, 0.0f, 0.0f));
					continue;
				}
			}
		}
	}
}



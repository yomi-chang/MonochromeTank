/**
 * @file   CollisionManager.cpp
 * @brief  当たり判定管理クラス
 */
#include "pch.h"
#include "Game/Other/CollisionManager.h"
#include "Game/Collider/BoxCollider.h"
#include "Game/Collider/SphereCollider.h"
#include "Game/Objects/Tank/TankBase/Tank.h"
#include "Game/Objects/Stage/Wall.h"
#include "Game/Objects/FixedTurret/FixedTurret.h"
#include "Game/Other/Parameter.h"

/// <summary>
/// コンストラクタ
/// </summary>
CollisionManager::CollisionManager()
	:
	m_tanks{},
	m_camera{},
	m_fixedTurrets{},
	m_walls{},
	m_wallGimmick{}
{
}

/// <summary>
/// デストラクタ
/// </summary>
CollisionManager::~CollisionManager()
{
}

/// <summary>
/// 更新処理
/// </summary>
void CollisionManager::Update()
{
	// 戦車の砲弾の当たり判定
	this->DetectCollisionTankAndCannonBall();

	// 戦車と連射弾の当たり判定
	this->DetectCollisionTankAndNomalBullets();

	// 戦車同士の当たり判定
	this->DetectCollisionTankAndOtherTanks();

	// 戦車と壁の当たり判定
	this->DetectCollisionTankAndWalls();

	// 弾と壁の当たり判定
	this->DetectCollisionBulletsAndWalls();

	// Rayと壁の当たり判定
	this->DetectCollisionRayAndWalls();

	// 壁と壁検知用コライダーの当たり判定
	this->DetectCollisionWallAndAvoidCollider();
}

/// <summary>
/// 戦車と連射弾
/// </summary>
void CollisionManager::DetectCollisionTankAndNomalBullets()
{
	for (auto& tank : m_tanks)
	{
		// 破壊されている場合
		if (tank->GetDead()) { continue; }

		// 他の戦車の弾
		for (auto& otherTank : m_tanks)
		{
			// 自機の場合か相手が破壊されている場合
			if (tank->GetTankNumber() == otherTank->GetTankNumber() ||
				otherTank->GetDead())
			{
				continue; 
			}

			for (auto& bullet : otherTank->GetCannon()->GetBullets())
			{
				// 弾丸が飛んでいる、かつ当たっているなら
				if (bullet->GetBulletState() == IBullet::FLYING &&
					tank->GetCollider()->CheckTriggerCollider(bullet->GetBoundingSphere()))
				{
					// 弾を使用済みにする
					bullet->SetBulletState(IBullet::USED);

					// ダメージ処理
					tank->Damage(Parameter::GetInstance()->GetBulletDamage());

					// 攻撃してきた戦車の情報を記憶する
					tank->SetTargetTank(otherTank);
				}
			}
		}

		// 固定砲台の弾
		for (auto& fixedTurret : m_fixedTurrets)
		{
			for (auto& bullet : fixedTurret->GetBullets())
			{
				// 弾丸が飛んでいる、かつ当たっているなら
				if (bullet->GetBulletState() == IBullet::FLYING &&
					tank->GetCollider()->CheckTriggerCollider(bullet->GetBoundingSphere()))
				{
					// 弾を使用済みにする
					bullet->SetBulletState(IBullet::USED);

					// ダメージ処理
					tank->Damage(Parameter::GetInstance()->GetBulletDamage());
				}
			}
		}
	}
}

/// <summary>
/// 戦車と砲弾
/// </summary>
void CollisionManager::DetectCollisionTankAndCannonBall()
{
	for (auto& tank : m_tanks)
	{
		// 破壊されている場合
		if (tank->GetDead()) { continue; }

		for (auto& otherTank : m_tanks)
		{
			// 自機の場合か相手が破壊されている場合
			if (tank->GetTankNumber() == otherTank->GetTankNumber() ||
				otherTank->GetDead()) 
			{
				continue;
			}

			// 他の戦車の弾が当たっていたらダメージ
			if (otherTank->GetCannon()->GetCannonBall()->GetBulletState() == IBullet::FLYING &&
				tank->GetCollider()->CheckTriggerCollider(otherTank->GetCannon()->GetCannonBall()->GetBoundingSphere()))
			{
				// 弾を使用済みにする
				otherTank->GetCannon()->GetCannonBall()->SetBulletState(IBullet::USED);

				// ダメージ処理
				tank->Damage(Parameter::GetInstance()->GetCannonBallDamage());

				// 攻撃してきた戦車の情報を記憶する
				tank->SetTargetTank(otherTank);
			}
		}
	}
}

/// <summary>
/// 戦車同士
/// </summary>
void CollisionManager::DetectCollisionTankAndOtherTanks()
{
	for (auto& tank : m_tanks)
	{
		// 破壊されている場合
		if (tank->GetHp() <= 0) { continue; }

		for (auto& otherTank : m_tanks)
		{
			// 自機の場合か相手が破壊されている場合
			if (tank->GetTankNumber() == otherTank->GetTankNumber() ||
				otherTank->GetHp() <= 0)
			{
				continue;
			}

			// 押し戻し処理
			DirectX::SimpleMath::Vector3 collisionVel = tank->GetCollider()->CheckCollisionCollider(otherTank->GetBoundingBox());
			collisionVel.y = 0.0f;
			otherTank->GetBody()->SetCollisionVel(collisionVel);
		}
	}
}

/// <summary>
/// 戦車と壁
/// </summary>
void CollisionManager::DetectCollisionTankAndWalls()
{
	for (auto& wall : m_walls)
	{
		for (auto& tank : m_tanks)
		{
			// 破壊されている場合
			if (tank->GetDead()) { continue; }

			// 壁のコライダー受け取り
			BoxCollider* wallCollider = wall->GetCollider();

			// 押し戻し処理
			DirectX::SimpleMath::Vector3 collisionVel = wallCollider->CheckCollisionCollider(tank->GetBoundingBox());
			collisionVel.y = 0.0f;
			tank->GetBody()->SetCollisionVel(collisionVel);
			
			// 壁に触れている間重力を無効にする
			if (wallCollider->CheckTriggerCollider(tank->GetBoundingBox()))
				tank->GetBody()->SetGravity(false);
		}
	}
}

/// <summary>
/// 弾と壁
/// </summary>
void CollisionManager::DetectCollisionBulletsAndWalls()
{
	for (auto& tank : m_tanks)
	{
		// 破壊されている場合
		if (tank->GetDead()) { continue; }

		for (auto& wall : m_walls)
		{
			// 壁のコライダー受け取り
			BoxCollider* wallCollider = wall->GetCollider();

			// 砲弾と壁の判定、当たっていたら使用済みに
			if (wallCollider->CheckTriggerCollider(tank->GetCannon()->GetCannonBall()->GetBoundingSphere()) &&
				tank->GetCannon()->GetCannonBall()->GetBulletState() == IBullet::FLYING)
			{
				tank->GetCannon()->GetCannonBall()->SetBulletState(IBullet::USED);
			}
			// 連射弾と壁の判定、当たっていたら使用済みに
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

/// <summary>
/// Rayと壁
/// </summary>
void CollisionManager::DetectCollisionRayAndWalls()
{
	using namespace DirectX::SimpleMath;

	Vector3 rayDirection;
	float rayDistance = 0.0f;
	float hitDistance = 0.0f;
	bool isHit = false;

	for (auto& tank : m_tanks)
	{
		// 破壊されている場合
		if (tank->GetDead()) { continue; }

		// プレイヤーのみ判定を行う
		if (tank->GetTankNumber() == 0) 
		{
			// Rayの作成
			Quaternion rotation = tank->GetCannon()->GetMuzzleRotation();
			Matrix matrix = Matrix::CreateFromQuaternion(rotation);
			rayDirection = matrix.Forward();
			rayDirection.Normalize();
			Ray ray{ tank->GetCannon()->GetMuzzlePosition(),rayDirection };
			// 射的距離の取得
			rayDistance = tank->GetCannon()->GetMaxRange();
			float minDistance = rayDistance;

			// 壁とRayの衝突
			for (auto& wall : m_walls)
			{
				isHit = ray.Intersects(*wall->GetBoundingBox(), hitDistance);

				if (isHit && hitDistance <= rayDistance)
				{
					// より短い距離を代入する
					minDistance = std::min(minDistance, hitDistance);
				}
			}
			// 敵とRayの衝突
			for (auto& otherTank : m_tanks)
			{
				// 自機の場合か相手が破壊されている場合
				if (tank->GetTankNumber() == otherTank->GetTankNumber() ||
					otherTank->GetDead())
				{
					continue;
				}

				isHit = ray.Intersects(*otherTank->GetBoundingBox(), hitDistance);
				if (isHit && hitDistance <= rayDistance)
				{
					// より短い距離を代入する
					minDistance = std::min(minDistance, hitDistance);
				}
			}

			// 衝突座標
			Vector3 hitPosition = Vector3::Zero;
			// 一回でも壁との衝突が取れていたら
			if (minDistance < rayDistance)
			{
				// 衝突点計算
				hitPosition = Vector3{ ray.position + ray.direction * minDistance - ray.direction };
				isHit = true;
			}
			else
			{
				// 照準画像の表示場所計算
				hitPosition = Vector3{ ray.position + ray.direction * rayDistance - ray.direction };

				// 床に埋まらないようにする
				if (hitPosition.y <= 0.0f)
				{
					hitPosition.y = 0.0f;
					isHit = true;
				}
			}
			// Rayの衝突情報の設定
			tank->GetCannon()->SetRayInfo(isHit, hitPosition);
		}
	}
}

/// <summary>
/// 壁と回避用コライダー
/// </summary>
void CollisionManager::DetectCollisionWallAndAvoidCollider()
{
	using namespace DirectX::SimpleMath;

	for (auto& tank : m_tanks)
	{
		// プレイヤーは判定を行わない
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



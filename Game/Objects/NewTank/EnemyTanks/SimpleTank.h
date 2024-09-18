#pragma once
#include "Interface/IObject.h"
#include "Game/Collider/SphereCollider.h"

class NewTank;
class SphereCollider;

class PlayerTank;
class EnemyHpGauge;

class SimpleTank : IObject
{
public:
	SimpleTank();

	~SimpleTank() override;

	void Initialize()override;

	void Update(float elapsedTime)override;
	void Update(
		float elapsedTime,
		const DirectX::SimpleMath::Vector3& currentPosition,
		const DirectX::SimpleMath::Quaternion& currentAngle
	)override {};

	void Render()override;
	void Finalize()override;

	void Attach(std::unique_ptr<IObject> parts)override;
	void Detach(std::unique_ptr<IObject> parts)override;

private:
	// 座標
	DirectX::SimpleMath::Vector3 m_position;
	// 回転角
	DirectX::SimpleMath::Quaternion m_angle;
	// 戦車
	std::unique_ptr<NewTank> m_tank;
	// コライダー
	std::unique_ptr<SphereCollider> m_collider;
	// HPゲージ
	std::unique_ptr<EnemyHpGauge> m_hpGauge;
	// ダメージ
	float m_damage;

private:
	// プレイヤーの情報
	PlayerTank* m_playerTank;

public:
	// 座標の取得
	DirectX::SimpleMath::Vector3 GetPosition() { return m_position; }
	// 回転角の取得
	DirectX::SimpleMath::Quaternion GetAngle() { return m_angle; }
	// コライダーの取得
	DirectX::BoundingSphere* GetBoundingSphere() { return m_collider->GetBoundingSphere(); }
	// プレイヤーの情報の受け取り
	void SetPlayerTank(PlayerTank* tank) { m_playerTank = tank; }

private:
	// 戦車と砲弾の衝突判定を行う
	void DetectCollisionTankAndBullets();
	// 戦車と戦車の衝突判定を行う
	void DetectCollisionTankAndOtherTanks();
};
#pragma once
#include "Interface/IObject.h"
#include "Game/Collider/BoxCollider.h"

class NewTank;
class BoxCollider;

class PlayerTank;
class EnemyHpGauge;

class SimpleTank : IObject
{
private:
	// 敵速度
	const float TANK_SPEED = 5.5f;

public:
	SimpleTank(
		DirectX::SimpleMath::Vector3 position
	);

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
	std::unique_ptr<BoxCollider> m_collider;
	// HPゲージ
	std::unique_ptr<EnemyHpGauge> m_hpGauge;
	// ダメージ
	float m_damage;
	// 死亡しているかどうか
	bool m_isDead;

	// 移動関係
	// 巡回地点
	std::vector<DirectX::SimpleMath::Vector3> m_patrolPoint;
	// 現在の番号
	unsigned int m_currentPoint;

	// デバッグ用モデル
	std::unique_ptr<DirectX::GeometricPrimitive> m_box;

private:
	// プレイヤーの情報
	PlayerTank* m_playerTank;

public:
	// 座標の取得
	DirectX::SimpleMath::Vector3 GetPosition() { return m_position; }
	// 回転角の取得
	DirectX::SimpleMath::Quaternion GetAngle() { return m_angle; }
	// コライダーの取得
	DirectX::BoundingBox* GetBoundingBox() { return m_collider->GetBoundingBox(); }
	// 死亡情報を渡す
	bool GetDead();
	
	
	// プレイヤーの情報の受け取り
	void SetPlayerTank(PlayerTank* tank) { m_playerTank = tank; }

private:
	// 戦車と砲弾の衝突判定を行う
	void DetectCollisionTankAndBullets();
	// 戦車と戦車の衝突判定を行う
	void DetectCollisionTankAndOtherTanks();
	// 巡回
	void Patrol(float elapsedTime);
};
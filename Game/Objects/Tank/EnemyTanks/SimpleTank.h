#pragma once
#include "Interface/IObject.h"
#include "Game/Collider/BoxCollider.h"

class Tank;
class BoxCollider;

class PlayerTank;
class EnemyHpGauge;

class EnemyTank : IObject
{
private:
	// 敵速度
	const float TANK_SPEED = 3.0f;

public:
	EnemyTank(
		int tankNumber,
		DirectX::SimpleMath::Vector3 position
	);

	~EnemyTank() override;

	void Initialize()override;

	void Update(float elapsedTime)override;

	void Render()override;
	void Finalize()override;

private:
	// 戦車番号
	int m_tankNumber;
	// 座標
	DirectX::SimpleMath::Vector3 m_position;
	// 回転角
	DirectX::SimpleMath::Quaternion m_angle;
	// 戦車
	std::unique_ptr<Tank> m_tank;
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
	// 座標の受け取り
	void SetPosition(DirectX::SimpleMath::Vector3 position);
	
	// プレイヤーの情報の受け取り
	void SetPlayerTank(PlayerTank* tank) { m_playerTank = tank; }
	// 戦車情報の取得
	Tank* GetTank() { return m_tank.get(); }
	// 他戦車情報の受け渡し
	void SetOtherTanks(std::vector<Tank*> tanks);

private:
	// 戦車と砲弾の衝突判定を行う
	void DetectCollisionTankAndBullets();
	// 戦車と戦車の衝突判定を行う
	void DetectCollisionTankAndOtherTanks();
	// 巡回
	void Patrol(float elapsedTime);
};
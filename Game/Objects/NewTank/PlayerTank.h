#pragma once
#include "Interface/IObject.h"
#include "Libraries/MyLib/FollowCamera.h"
#include "Game/Collider/BoxCollider.h"

#include "Game/Objects/NewTank/NewTankBase/NewTankCannon.h"

class NewTank;
class BoxCollider;
class HpGauge;
class Wall;

namespace mylib
{
	class FollowCamera;
}

class PlayerTank : IObject
{
private:
	// 砲塔の回転制限
	const float TURRET_ANGLE_MIN = DirectX::XMConvertToRadians(-25.0f);
	const float TURRET_ANGLE_MAX = DirectX::XMConvertToRadians(25.0f);
	// 砲身の回転制限
	const float CANNON_ANGLE_MIN = DirectX::XMConvertToRadians(-7.5f);
	const float CANNON_ANGLE_MAX = DirectX::XMConvertToRadians(7.5f);

	const DirectX::SimpleMath::Vector3 COLLIDER_SIZE = DirectX::SimpleMath::Vector3(1.2f, 1.0f, 1.2f);
	const float COLLIDER_POSITION = 0.25f;

public:
	PlayerTank();

	~PlayerTank()override;

	void Initialize()override;

	void Update(float elapsedTime)override;
	void Update(
		float elapsedTime,
		const DirectX::SimpleMath::Vector3& currentPosition,
		const DirectX::SimpleMath::Quaternion& currentAngle
	)override{};

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
	std::unique_ptr<HpGauge> m_hpGauge;
	// ダメージ
	float m_damege;

private:
	void KeyBoardEvent(float elapsedTime);
	// 移動処理
	void Move(float elapsedTime);

	// 回転処理
	void RotateTurretCannon();

public:
	// 壁情報の受け取り
	void SetWalls(std::vector<Wall*> walls);

	// 座標の取得
	DirectX::SimpleMath::Vector3 GetPosition() { return m_position; }
	// 回転角の取得
	DirectX::SimpleMath::Quaternion GetAngle() { return m_angle; }

	// 座標の受け取り
	void SetPosition(DirectX::SimpleMath::Vector3 position);

	// コライダーの取得
	DirectX::BoundingBox* GetBoundingBox() { return m_collider->GetBoundingBox(); }

	// 砲身の取得
	NewTankCannon* GetTankCannon();
};
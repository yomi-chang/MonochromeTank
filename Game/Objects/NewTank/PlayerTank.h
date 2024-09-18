#pragma once
#include "Interface/IObject.h"

class NewTank;
class SphereCollider;

class Wall;

class PlayerTank : IObject
{
private:
	// –C“ƒ‚Ì‰ñ“]§ŒÀ
	const float TURRET_ANGLE_MIN = DirectX::XMConvertToRadians(-90.0f);
	const float TURRET_ANGLE_MAX = DirectX::XMConvertToRadians(90.0f);
	// –Cg‚Ì‰ñ“]§ŒÀ
	const float CANNON_ANGLE_MIN = DirectX::XMConvertToRadians(-10.0f);
	const float CANNON_ANGLE_MAX = DirectX::XMConvertToRadians(15.0f);

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
	// íÔ
	std::unique_ptr<NewTank> m_tank;
	// ƒRƒ‰ƒCƒ_[
	std::unique_ptr<SphereCollider> m_collider;

private:
	// •Çî•ñ
	std::vector<Wall*> m_walls;

private:
	// ˆÚ“®ˆ—
	void Move(float elapsedTime);

	// ‰ñ“]ˆ—
	void RotateTurretCannon();
};
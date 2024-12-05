#pragma once

class SphereCollider;
class PlayerTank;

class StageItem
{
private:
	// アイテムの種類
	enum ItemType
	{
		Heel,
		Attack,
		Stealth
	};

public:
	StageItem();

	~StageItem() = default;

	void Initialize(DirectX::SimpleMath::Vector3 position);

	void Update();

	void Render();

private:
	// どのアイテムか
	ItemType itemType;

	// コライダー
	std::unique_ptr<SphereCollider> m_collider;
};
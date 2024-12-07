#pragma once

class BoxCollider;

class WallGimmick
{
public:
	WallGimmick();

	~WallGimmick() = default;

	void Initialize();

	void Update();

	void Render();

private:
	// コライダー
	std::unique_ptr<BoxCollider> m_collider;

	// 弾があたっているか
	bool m_isHit;

	// 座標
	DirectX::SimpleMath::Vector3 m_position;

	// デバッグ用モデル
	std::unique_ptr<DirectX::GeometricPrimitive> m_box;

public:
	// あたっているかの情報の取得
	bool GetHit() { return m_isHit; }

};
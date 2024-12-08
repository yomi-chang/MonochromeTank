#pragma once

class BoxCollider;
class Tank;

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

	// 弾情報を取得するための戦車クラス
	std::vector<Tank*> m_tanks;

public:
	// あたっているかの情報の取得
	bool GetHit() { return m_isHit; }

	// 戦車情報の設定
	void SetTanks(std::vector<Tank*> tanks) { m_tanks = tanks; }

};
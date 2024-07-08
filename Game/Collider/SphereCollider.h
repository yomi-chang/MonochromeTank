#pragma once
#include "Framework/Graphics.h"

class SphereCollider
{
public:
	DirectX::BoundingSphere GetBoundingSphere() { return m_boundingSphere; }

public:
	// コンストラクタ
	SphereCollider();

	// バウンディングスフィアの作成
	void CreateBoundingSphere(DirectX::SimpleMath::Vector3 centerPosition, float radius);

	// 更新処理
	void Update(DirectX::SimpleMath::Vector3 centerPosition);

	// 描画処理
	void Render();

	// 当たっているかどうかの判定
	bool ChackHitBoundingSphere(DirectX::BoundingSphere* boundingSphere);
	bool ChackHitBoundingBox(DirectX::BoundingBox* boundingBox);

private:
	// グラフィックス
	Graphics* m_graphics;

	// バウンディングスフィア
	DirectX::BoundingSphere m_boundingSphere;
};
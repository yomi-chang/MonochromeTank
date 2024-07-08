#pragma once
#include "Framework/Graphics.h"

class BoxCollider
{
public:
	DirectX::BoundingBox GetBoundingBox() { return m_boundingBox; }

public:
	// コンストラクタ
	BoxCollider();

	// バウンディングスフィアの作成
	void CreateBoundingBox(DirectX::SimpleMath::Vector3 centerPosition, DirectX::SimpleMath::Vector3 size);

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
	DirectX::BoundingBox m_boundingBox;
};
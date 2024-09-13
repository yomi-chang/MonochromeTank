#pragma once
#include "Framework/Graphics.h"

class BoxCollider
{
public:
	DirectX::BoundingBox* GetBoundingBox() { return &m_boundingBox; }

public:
	// コンストラクタ
	BoxCollider();

	// バウンディングスフィアの作成
	void CreateBoundingBox(DirectX::SimpleMath::Vector3 centerPosition, DirectX::SimpleMath::Vector3 size);

	// 更新処理
	void Update(DirectX::SimpleMath::Vector3 centerPosition);

	// 描画処理
	void Render(DirectX::XMVECTOR color = DirectX::Colors::Aqua);

	// 当たっているかどうかの判定
	DirectX::SimpleMath::Vector3 CheckCollisionCollider(DirectX::BoundingSphere* boundingSphere);
	DirectX::SimpleMath::Vector3 CheckCollisionCollider(DirectX::BoundingBox* boundingBox);
	bool CheckTriggerCollider(DirectX::BoundingSphere* boundingSphere);
	bool CheckTriggerCollider(DirectX::BoundingBox* boundingBox);

private:
	// グラフィックス
	Graphics* m_graphics;

	// バウンディングスフィア
	DirectX::BoundingBox m_boundingBox;
};
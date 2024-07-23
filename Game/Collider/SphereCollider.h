#pragma once
#include "Framework/Graphics.h"

class SphereCollider
{
public:
	DirectX::BoundingSphere* GetBoundingSphere() { return &m_boundingSphere; }

public:
	// コンストラクタ
	SphereCollider();

	// バウンディングスフィアの作成
	void CreateBoundingSphere(DirectX::SimpleMath::Vector3 position, float radius);

	// 更新処理
	void Update(DirectX::SimpleMath::Vector3 position);

	// 描画処理
	void Render();

	// 当たっているかどうかの判定
	DirectX::SimpleMath::Vector3 CheckCollisionCollider(DirectX::BoundingSphere* boundingSphere);
	DirectX::SimpleMath::Vector3 CheckCollisionCollider(DirectX::BoundingBox* boundingBox);
	bool CheckTriggerCollider(DirectX::BoundingSphere* boundingSphere);
	bool CheckTriggerCollider(DirectX::BoundingBox* boundingBox);

private:
	// グラフィックス
	Graphics* m_graphics;

	// プリミティブバッチ
	//std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;

	// バウンディングスフィア
	DirectX::BoundingSphere m_boundingSphere;
};
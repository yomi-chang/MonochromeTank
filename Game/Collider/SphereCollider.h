/*
	@file	SphereCollider.h
	@brief	球状の当たり判定クラス
*/
#pragma once
#include "Framework/Graphics.h"

class SphereCollider
{
public:
	// コンストラクタ
	SphereCollider();
	// デストラクタ
	~SphereCollider();
	// バウンディングスフィアの作成
	void CreateBoundingSphere(DirectX::SimpleMath::Vector3 position, float radius);
	// 更新処理
	void Update(DirectX::SimpleMath::Vector3 position);
	// 描画処理
	void Render(DirectX::XMVECTOR color = DirectX::Colors::Aqua);

private:
	// グラフィックス
	Graphics* m_graphics;
	// バウンディングスフィア
	DirectX::BoundingSphere m_boundingSphere;

public:
	// バウンディングスフィアの取得
	DirectX::BoundingSphere* GetBoundingSphere() { return &m_boundingSphere; }
	// 当たっているかどうかの判定
	DirectX::SimpleMath::Vector3 CheckCollisionCollider(DirectX::BoundingSphere* boundingSphere);
	DirectX::SimpleMath::Vector3 CheckCollisionCollider(DirectX::BoundingBox* boundingBox);
	bool CheckTriggerCollider(DirectX::BoundingSphere* boundingSphere);
	bool CheckTriggerCollider(DirectX::BoundingBox* boundingBox);
};
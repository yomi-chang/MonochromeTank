/**
 * @file   BoxCollider.h
 * @brief  箱状の当たり判定クラス
 */
#pragma once
#include "Framework/Graphics.h"

class BoxCollider
{
public:
	// コンストラクタ
	BoxCollider();
	// デストラクタ
	~BoxCollider();
	// バウンディングボックスの作成
	void CreateBoundingBox(DirectX::SimpleMath::Vector3 centerPosition, DirectX::SimpleMath::Vector3 size);
	// 更新処理
	void Update(DirectX::SimpleMath::Vector3 centerPosition);
	// 描画処理
	void Render(DirectX::XMVECTOR color = DirectX::Colors::Aqua);

private:
	// グラフィックス
	Graphics* m_graphics;
	// バウンディングボックス
	DirectX::BoundingBox m_boundingBox;

public:
	// バウンディングボックス取得
	DirectX::BoundingBox* GetBoundingBox() { return &m_boundingBox; }
	// 当たっているかどうかの判定
	DirectX::SimpleMath::Vector3 CheckCollisionCollider(DirectX::BoundingSphere* boundingSphere);
	DirectX::SimpleMath::Vector3 CheckCollisionCollider(DirectX::BoundingBox* boundingBox);
	bool CheckTriggerCollider(DirectX::BoundingSphere* boundingSphere);
	bool CheckTriggerCollider(DirectX::BoundingBox* boundingBox);
};
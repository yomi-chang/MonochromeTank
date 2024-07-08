#include "pch.h"
#include "Game/Collider/BoxCollider.h"
#include "Libraries/Microsoft/DebugDraw.h"

/// <summary>
/// コンストラクタ
/// </summary>
BoxCollider::BoxCollider()
	:
	m_boundingBox{},
	m_graphics{Graphics::GetInstance()}
{
}

/// <summary>
/// バウンディングボックスの作成
/// </summary>
/// <param name="centerPosition">中心座標</param>
/// <param name="radius">半径</param>
void BoxCollider::CreateBoundingBox(DirectX::SimpleMath::Vector3 centerPosition, DirectX::SimpleMath::Vector3 size)
{
	// 中心座標の設定
	m_boundingBox.Center = centerPosition;

	// 半径の設定
	m_boundingBox.Extents = size;
}

/// <summary>
/// コライダーの更新
/// </summary>
/// <param name="centerPosition">中心座標</param>
void BoxCollider::Update(DirectX::SimpleMath::Vector3 centerPosition)
{
	// 中心座標の更新
	m_boundingBox.Center = centerPosition;
}


/// <summary>
/// 描画処理
/// </summary>
void BoxCollider::Render()
{
	// 当たり判定の表示
	m_graphics->GetPrimitiveBatch()->Begin();
	DX::Draw(Graphics::GetInstance()->GetPrimitiveBatch(), m_boundingBox);
	m_graphics->GetPrimitiveBatch()->End();
}

/// <summary>
/// ボックスと球の当たり判定
/// </summary>
/// <param name="boundingSphere">相手のスフィアコライダー</param>
/// <returns>当たったかどうか</returns>
bool BoxCollider::ChackHitBoundingSphere(DirectX::BoundingSphere* boundingSphere)
{
	// 当たっている場合
	if (m_boundingBox.Intersects(*boundingSphere))
	{
		return true;
	}

	//当たっていない場合
	return false;
}

/// <summary>
/// ボックスとボックスの当たり判定
/// </summary>
/// <param name="boundingBox">相手のボックスコライダー</param>
/// <returns>当たったかどうか</returns>
bool BoxCollider::ChackHitBoundingBox(DirectX::BoundingBox* boundingBox)
{
	// 当たっている場合
	if (m_boundingBox.Intersects(*boundingBox))
	{
		return true;
	}

	//当たっていない場合
	return false;
}

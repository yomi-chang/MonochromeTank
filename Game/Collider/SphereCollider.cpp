#include "pch.h"
#include "Game/Collider/SphereCollider.h"
#include "Libraries/Microsoft/DebugDraw.h"

/// <summary>
/// コンストラクタ
/// </summary>
SphereCollider::SphereCollider()
	:
	m_boundingSphere{},
	m_graphics{Graphics::GetInstance()}
{
}

/// <summary>
/// バウンディングスフィアの作成
/// </summary>
/// <param name="centerPosition">中心座標</param>
/// <param name="radius">半径</param>
void SphereCollider::CreateBoundingSphere(DirectX::SimpleMath::Vector3 centerPosition, float radius)
{
	// 中心座標の設定
	m_boundingSphere.Center = centerPosition;

	// 半径の設定
	m_boundingSphere.Radius = radius;
}

/// <summary>
/// コライダーの更新
/// </summary>
/// <param name="centerPosition">中心座標</param>
void SphereCollider::Update(DirectX::SimpleMath::Vector3 centerPosition)
{
	// 中心座標の更新
	m_boundingSphere.Center = centerPosition;
}


/// <summary>
/// 描画処理
/// </summary>
void SphereCollider::Render()
{
	// 当たり判定の表示
	m_graphics->GetPrimitiveBatch()->Begin();
	DX::Draw(Graphics::GetInstance()->GetPrimitiveBatch(), m_boundingSphere);
	m_graphics->GetPrimitiveBatch()->End();
}

/// <summary>
/// 球と球の当たり判定
/// </summary>
/// <param name="boundingSphere">相手のスフィアコライダー</param>
/// <returns>当たったかどうか</returns>
bool SphereCollider::ChackHitBoundingSphere(DirectX::BoundingSphere* boundingSphere)
{
	// 当たっている場合
	if (m_boundingSphere.Intersects(*boundingSphere))
	{
		return true;
	}

	//当たっていない場合
	return false;
}

/// <summary>
/// 球とボックスの当たり判定
/// </summary>
/// <param name="boundingBox">相手のボックスコライダー</param>
/// <returns>当たったかどうか</returns>
bool SphereCollider::ChackHitBoundingBox(DirectX::BoundingBox* boundingBox)
{
	// 当たっている場合
	if (m_boundingSphere.Intersects(*boundingBox))
	{
		return true;
	}

	//当たっていない場合
	return false;
}

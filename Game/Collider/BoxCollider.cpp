#include "pch.h"
#include "Game/Collider/BoxCollider.h"
#include "Libraries/Microsoft/DebugDraw.h"

/// <summary>
/// コンストラクタ
/// </summary>
BoxCollider::BoxCollider()
	:
	m_boundingBox{},
	m_graphics{ Graphics::GetInstance() }
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

	// サイズの設定
	m_boundingBox.Extents = size / 2;
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
void BoxCollider::Render(DirectX::XMVECTOR color)
{
	// 当たり判定の表示(デバッグビルドでのみ表示)
#ifdef _DEBUG
	// プリミティブ描画を開始する
	m_graphics->DrawPrimitiveBegin(m_graphics->GetViewMatrix(), m_graphics->GetProjectionMatrix());
	// 境界ボックスを描画する
	DX::Draw(m_graphics->GetPrimitiveBatch(), m_boundingBox, color);
	// プリミティブ描画を終了する
	m_graphics->DrawPrimitiveEnd();
#endif
}

// 当たり判定の処理==================================================================================
/// <summary>
/// バウンディングスフィアとの押し戻しありの当たり判定
/// </summary>
/// <param name="boundingSphere">相手のバウンディングスフィア</param>
/// <returns>押し戻し距離</returns>
DirectX::SimpleMath::Vector3 BoxCollider::CheckCollisionCollider(DirectX::BoundingSphere* boundingSphere)
{
	using namespace DirectX::SimpleMath;
	if (!m_boundingBox.Intersects(*boundingSphere)) { return Vector3::Zero; }

	// AABB用のmin/maxを計算する
	Vector3 aMin = m_boundingBox.Center - m_boundingBox.Extents;
	Vector3 aMax = m_boundingBox.Center + m_boundingBox.Extents;
	Vector3 bMin = boundingSphere->Center - Vector3(boundingSphere->Radius, boundingSphere->Radius, boundingSphere->Radius);
	Vector3 bMax = boundingSphere->Center + Vector3(boundingSphere->Radius, boundingSphere->Radius, boundingSphere->Radius);

	// 各軸の差分を計算する
	float dx1 = aMin.x - bMax.x;
	float dx2 = aMax.x - bMin.x;
	float dy1 = aMin.y - bMax.y;
	float dy2 = aMax.y - bMin.y;
	float dz1 = aMin.z - bMax.z;
	float dz2 = aMax.z - bMin.z;

	// 各軸について、絶対値の小さい方を軸のめり込み量とする：AABBの重なった部分を特定する
	float dx = abs(dx1) < abs(dx2) ? dx1 : dx2;
	float dy = abs(dy1) < abs(dy2) ? dy1 : dy2;
	float dz = abs(dz1) < abs(dz2) ? dz1 : dz2;

	// 押し戻しベクトル
	Vector3 pushBackVec = Vector3::Zero;

	// めり込みが一番小さい軸を押し戻す
	if (abs(dx) <= abs(dy) && abs(dx) <= abs(dz))
	{
		pushBackVec.x += dx;
	}
	else if (abs(dz) <= abs(dx) && abs(dz) <= abs(dy))
	{
		pushBackVec.z += dz;
	}
	else
	{
		pushBackVec.y += dy;
	}

	// 押し戻す
	boundingSphere->Center = boundingSphere->Center + pushBackVec;
	return pushBackVec;
}

/// <summary>
/// バウンディングボックスとの押し戻しありの当たり判定
/// </summary>
/// <param name="boundingBox">相手のバウンディングボックス</param>
/// <returns>押し戻し距離</returns>
DirectX::SimpleMath::Vector3 BoxCollider::CheckCollisionCollider(DirectX::BoundingBox* boundingBox)
{
	using namespace DirectX::SimpleMath;
	if (!m_boundingBox.Intersects(*boundingBox)) { return Vector3::Zero; }

	// AABB用のmin/maxを計算する
	Vector3 aMin = boundingBox->Center - boundingBox->Extents;
	Vector3 aMax = boundingBox->Center + boundingBox->Extents;
	Vector3 bMin = m_boundingBox.Center - m_boundingBox.Extents;
	Vector3 bMax = m_boundingBox.Center + m_boundingBox.Extents;

	// 各軸の差分を計算する
	float dx1 = bMax.x - aMin.x;
	float dx2 = bMin.x - aMax.x;
	float dy1 = bMax.y - aMin.y;
	float dy2 = bMin.y - aMax.y;
	float dz1 = bMax.z - aMin.z;
	float dz2 = bMin.z - aMax.z;

	// 各軸について、絶対値の小さい方を軸のめり込み量とする：AABBの重なった部分を特定する
	float dx = abs(dx1) < abs(dx2) ? dx1 : dx2;
	float dy = abs(dy1) < abs(dy2) ? dy1 : dy2;
	float dz = abs(dz1) < abs(dz2) ? dz1 : dz2;

	// 押し戻しベクトル
	Vector3 pushBackVec = Vector3::Zero;

	// めり込みが一番小さい軸を押し戻す
	if (abs(dx) <= abs(dy) && abs(dx) <= abs(dz))
	{
		pushBackVec.x += dx;
	}
	else if (abs(dz) <= abs(dx) && abs(dz) <= abs(dy))
	{
		pushBackVec.z += dz;
	}
	else
	{
		pushBackVec.y += dy;
	}

	// 押し戻す
	boundingBox->Center = boundingBox->Center + pushBackVec;

	return pushBackVec;
}

/// <summary>
/// バウンディングスフィアとの当たり判定
/// </summary>
/// <param name="boundingSphere">相手のバウンディングスフィア</param>
/// <returns>当たっているかどうか</returns>
bool BoxCollider::CheckTriggerCollider(DirectX::BoundingSphere* boundingSphere)
{
	if (!m_boundingBox.Intersects(*boundingSphere)) { return false; }

	// 当たっていたらtrueを返す
	return true;
}

/// <summary>
/// バウンディングボックスとの当たり判定
/// </summary>
/// <param name="boundingSphere">相手のバウンディングボックス</param>
/// <returns>当たっているかどうか</returns>
bool BoxCollider::CheckTriggerCollider(DirectX::BoundingBox* boundingBox)
{
	if (!m_boundingBox.Intersects(*boundingBox)) { return false; }

	// 当たっていたらtrueを返す
	return true;
}
//==================================================================================================


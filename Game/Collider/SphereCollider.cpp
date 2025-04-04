/*
	@file	SphereCollider.cpp
	@brief	球状の当たり判定クラス
*/
#include "pch.h"
#include "Game/Collider/SphereCollider.h"
#include "Libraries/Microsoft/DebugDraw.h"

/// <summary>
/// コンストラクタ
/// </summary>
SphereCollider::SphereCollider()
	:
	m_boundingSphere{},
	m_graphics{ Graphics::GetInstance() }
{
}

/// <summary>
/// バウンディングスフィアの作成
/// </summary>
/// <param name="centerPosition">中心座標</param>
/// <param name="radius">半径</param>
void SphereCollider::CreateBoundingSphere(DirectX::SimpleMath::Vector3 position, float radius)
{
	// 中心座標の設定
	m_boundingSphere.Center = position;

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
void SphereCollider::Render(DirectX::XMVECTOR color)
{
	// 当たり判定の表示(デバッグビルドでのみ表示)
#ifdef _DEBUG
	// プリミティブ描画を開始する
	m_graphics->DrawPrimitiveBegin(m_graphics->GetViewMatrix(), m_graphics->GetProjectionMatrix());
	// 境界ボックスを描画する
	DX::Draw(m_graphics->GetPrimitiveBatch(), m_boundingSphere, color);
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
DirectX::SimpleMath::Vector3 SphereCollider::CheckCollisionCollider(DirectX::BoundingSphere* boundingSphere)
{
	using namespace DirectX::SimpleMath;
	if (!m_boundingSphere.Intersects(*boundingSphere)) { return Vector3::Zero; }

	// Ａの中心とＢの中心との差分ベクトル（ＢからＡに向かうベクトル）…①
	Vector3 diffVec = m_boundingSphere.Center - boundingSphere->Center;

	// Ａの中心とＢの中心との距離（①の長さ）…②
	float diffLength = diffVec.Length();
	// Ａの半径とＢの半径の合計…③
	float sumRadius = +m_boundingSphere.Radius + boundingSphere->Radius;
	// （ＡがＢに）めり込んだ距離（③－②）…④
	diffLength = sumRadius - diffLength;
	// ①を正規化する…⑤
	diffVec.Normalize();
	// 押し戻すベクトルを計算する（⑤と④で表現する）…⑥
	diffVec *= diffLength;

	// ⑥を使用して、Ａの座標とＡのコライダー座標を更新する（実際に押し戻す）
	m_boundingSphere.Center = m_boundingSphere.Center + diffVec;
	return diffVec;
}

/// <summary>
/// バウンディングボックスとの押し戻しありの当たり判定
/// </summary>
/// <param name="boundingBox">相手のバウンディングボックス</param>
/// <returns>押し戻し距離</returns>
DirectX::SimpleMath::Vector3 SphereCollider::CheckCollisionCollider(DirectX::BoundingBox* boundingBox)
{
	using namespace DirectX::SimpleMath;
	return Vector3::Zero;

	//ToDo 押し戻しの追加
}

/// <summary>
/// バウンディングスフィアとの当たり判定
/// </summary>
/// <param name="boundingSphere">相手のバウンディングスフィア</param>
/// <returns>当たっているかどうか</returns>
bool SphereCollider::CheckTriggerCollider(DirectX::BoundingSphere* boundingSphere)
{
	if (!m_boundingSphere.Intersects(*boundingSphere)) { return false; }

	// 当たっていたらtrueを返す
	return true;
}

/// <summary>
/// バウンディングボックスとの当たり判定
/// </summary>
/// <param name="boundingBox">相手のバウンディングボックス</param>
/// <returns>当たっているかどうか</returns>
bool SphereCollider::CheckTriggerCollider(DirectX::BoundingBox* boundingBox)
{
	if (!m_boundingSphere.Intersects(*boundingBox)) { return false; }

	// 当たっていたらtrueを返す
	return true;
}
//==================================================================================================
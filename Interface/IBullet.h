#pragma once
#include "pch.h"

// IBulletインターフェース
class IBullet
{
public:
	// 砲弾の状態
	enum BulletState : short { UNUSED, FLYING, USED };

public:
	// 現在の位置を取得する
	virtual DirectX::SimpleMath::Vector3 GetPosition() const = 0;
	// 現在の位置を設定する
	virtual void SetPosition(const DirectX::SimpleMath::Vector3& position) = 0;
	// 砲弾回転角の設定
	virtual void SetRotation(const DirectX::SimpleMath::Quaternion& rotation) = 0;
	// 砲弾が発射されているかどうかを取得する
	virtual BulletState GetBulletState() const = 0;
	// 砲弾の状態を設定する
	virtual void SetBulletState(BulletState bulletState) = 0;

	// コライダーの取得
	virtual DirectX::BoundingSphere* GetBoundingSphere() = 0;

	// コライダーの座標の設定
	virtual void SetColliderPosition(DirectX::SimpleMath::Vector3 position) = 0;

public:
	// 仮想デストラクタ
	virtual ~IBullet() = default;
	// 初期化する
	virtual void Initialize() = 0;
	// 更新する
	virtual void Update(float time) = 0;
	// 描画する
	virtual void Render() = 0;
	// 後処理を行う
	virtual void Finalize() = 0;
};
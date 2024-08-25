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
	// 砲弾上下回転角を取得する 
	virtual float GetAngleUD() const = 0;
	// 砲弾上下回転角を設定する 
	virtual void SetAngleUD(const float& angleUD) = 0;
	// 砲弾左右回転角を取得する 
	virtual float GetAngleRL() const = 0;
	// 砲弾左右回転角を設定する
	virtual void SetAngleRL(const float& angleRL) = 0;
	// 砲弾が発射されているかどうかを取得する
	virtual BulletState GetBulletState() const = 0;
	// 砲弾の状態を設定する
	virtual void SetBulletState(BulletState bulletState) = 0;

	// コライダーの取得
	virtual DirectX::BoundingSphere* GetBoundingSphere() = 0;

	// 経過時間の取得
	virtual float GetTime() = 0;
	// 経過時間のリセット
	virtual void ResetElapsedTime() = 0;
	

	// モデルを取得する
	//virtual DirectX::Model* GetModel() = 0;
	// ワールド境界ボックスを取得する
	//virtual DirectX::BoundingBox* GetWorldBoundingBox() = 0;

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
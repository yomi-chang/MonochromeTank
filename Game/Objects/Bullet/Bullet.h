/*
	@file	Bullet.h
	@brief	連射弾クラス
*/
#pragma once
#include "Interface/IBullet.h"
#include "Framework/Graphics.h"
#include "Game/Collider/SphereCollider.h"

class BulletTrail;

class Bullet : public IBullet
{
public:
	// コンストラクタ
	Bullet(IBullet::BulletState bulletState);
	// デストラクタ
	~Bullet();
	// 初期化処理
	void Initialize();
	// 更新処理 
	inline void Update(float time);
	// 描画処理 
	inline void Render();
	// 弾描画処理
	inline void DrawBullet();
	// 終了処理 
	void Finalize();

private:
	// グラフィックス
	Graphics* m_graphics;
	// 速度
	DirectX::SimpleMath::Vector3 m_velocity;
	// 位置
	DirectX::SimpleMath::Vector3 m_position;
	// 角度
	DirectX::SimpleMath::Quaternion m_rotation;
	// ワールド行列
	DirectX::SimpleMath::Matrix m_worldMatrix;
	// 弾が発射されているかどうかを示す
	BulletState m_bulletState;
	// スフィアコライダー
	std::unique_ptr<SphereCollider> m_collider;
	// ジオメトリックプリミティブ
	std::unique_ptr<DirectX::GeometricPrimitive> m_bullet;
	// トレイル
	std::unique_ptr<BulletTrail> m_trail;
	// 破壊カウント
	float m_count;

public:
	// 位置を取得する 
	DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
	// 位置を設定する
	virtual void SetPosition(const DirectX::SimpleMath::Vector3& position) { m_position = position; }
	// 弾の回転角の設定
	void SetRotation(const DirectX::SimpleMath::Quaternion& rotation) { m_rotation = rotation; }
	// 弾が発射されているかどうかを取得する
	IBullet::BulletState GetBulletState() const { return m_bulletState; }
	// 弾の状態を設定する
	void SetBulletState(IBullet::BulletState bulletState) { m_bulletState = bulletState; }
	// コライダーの取得
	DirectX::BoundingSphere* GetBoundingSphere() { return m_collider->GetBoundingSphere(); }
	// コライダー座標の設定
	void SetColliderPosition(DirectX::SimpleMath::Vector3 position) { m_collider->Update(position); }
};
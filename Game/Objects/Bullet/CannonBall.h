#pragma once
#include "Interface/IBullet.h"
#include "Framework/Graphics.h"
#include "Game/Collider/SphereCollider.h"

class CannonBall : public IBullet
{
public:
	// 砲弾スピードを定義する
	static const DirectX::SimpleMath::Vector3 SPEED;

	static const DirectX::SimpleMath::Vector3 GRAVITY;

public:
	// 位置を取得する 
	DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
	// 位置を設定する
	virtual void SetPosition(const DirectX::SimpleMath::Vector3& position) { m_position = position; };
	// 砲弾上下回転角を取得する 
	float GetAngleUD() const { return m_angleUD; }
	// 砲弾上下回転角を設定する 
	void SetAngleUD(const float& angleUD) { m_angleUD = angleUD; }
	// 砲弾左右回転角を取得する 
	float GetAngleRL() const { return m_angleRL; }
	// 砲弾左右回転角を設定する
	void SetAngleRL(const float& angleRL) { m_angleRL = angleRL; }
	// 砲弾が発射されているかどうかを取得する
	IBullet::BulletState GetBulletState() const { return m_bulletState; }
	// 砲弾の状態を設定する
	void SetBulletState(IBullet::BulletState bulletState) { m_bulletState = bulletState; }
	// コライダーの取得
	DirectX::BoundingSphere* GetBoundingSphere() { return m_collider->GetBoundingSphere(); }

	// コライダー座標の設定
	void SetColliderPosition(DirectX::SimpleMath::Vector3 position) { m_collider->Update(position); }

public:
	// コンストラクタ
	CannonBall(IBullet::BulletState bulletState);
	// デストラクタ
	~CannonBall();
	// 初期化する
	void Initialize();
	// Bulletオブジェクトを更新する 
	inline void Update(float time);
	// Bulletオブジェクトを描画する 
	inline void Render();
	// 砲弾を描画する
	inline void DrawBullet();
	// Bulletオブジェクトの後処理をおこなう 
	void Finalize();

private:
	// グラフィックス
	Graphics* m_graphics;
	// 速度
	DirectX::SimpleMath::Vector3 m_velocity;
	// 位置
	DirectX::SimpleMath::Vector3 m_position;
	// 上下角度
	float m_angleUD;
	// 左右角度
	float m_angleRL;
	// ワールド行列
	DirectX::SimpleMath::Matrix m_worldMatrix;
	// 砲弾が発射されているかどうかを示す
	BulletState m_bulletState;
	// スフィアコライダー
	std::unique_ptr<SphereCollider> m_collider;
	// ジオメトリックプリミティブ
	std::unique_ptr<DirectX::GeometricPrimitive> m_bullet;

	float m_elapsedTime;
};
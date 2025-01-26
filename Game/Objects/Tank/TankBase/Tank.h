#pragma once
#include "Interface/IParent.h"
#include "Interface/IParts.h"

#include "Game/Objects/Tank/TankBase/TankBody.h"
#include "Game/Objects/Tank/TankBase/TankTurret.h"
#include "Game/Objects/Tank/TankBase/TankCannon.h"

#include "Game/Collider/BoxCollider.h"
#include "Game/Collider/SphereCollider.h"

class TankBody;
class TankTurret;
class TankCannon;

class BoxCollider;
class SphereCollider;

class Smoke;

class Tank : public IParent
{
private:
	// コライダーのサイズ
	const DirectX::SimpleMath::Vector3 COLLIDER_SIZE = DirectX::SimpleMath::Vector3(1.3f, 0.7f, 1.3f);
	// コライダーを上にずらす
	const float COLLIDER_POSITION = 0.5f;
	// 体力
	const float DEFAULT_HP = 10;
public:
	// コンストラクタ
	Tank(
		const int& tankNumber,
		const DirectX::SimpleMath::Vector3& initialPosition,
		const float& initialAngle
	);
	// デストラクタ
	~Tank() override;
	// 初期化処理
	void Initialize() override;
	// 更新処理
	void Update(float elapsedTime)override;
	// 描画処理
	void Render() override;
	// 終了処理
	void Finalize() override;

	// パーツ追加
	void Attach(std::unique_ptr<IParts> parts) override;
	// パーツ削除
	void Detach(std::unique_ptr<IParts> parts) override;

private:
	Graphics* m_graphics;									// グラフィックス
	DirectX::SimpleMath::Vector3 m_initialPosition;			// 初期座標
	DirectX::SimpleMath::Quaternion m_initialRotation;		// 初期回転角
	DirectX::SimpleMath::Vector3 m_currentPosition;			// 現在の座標
	DirectX::SimpleMath::Quaternion m_currentRotation;		// 現在の回転角
	std::vector<std::unique_ptr<IParts>> m_tankParts;		// 自身が管理する戦車部品の配列
	DirectX::SimpleMath::Matrix m_worldMatrix;				// ワールド行列

	// 各パーツ
	TankBody* m_body;
	TankTurret* m_turret;
	TankCannon* m_cannon;

	// 影描画用
	DirectX::VertexPositionTexture m_vertices[4];												// 影用ポリゴン
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch;	// プリミティブバッチ
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;									// 入力レイアウト 
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;										// エフェクト

	int m_tankNumber;										// 戦車番号
	std::unique_ptr<BoxCollider> m_collider;				// コライダー
	std::unique_ptr<SphereCollider> m_avoidCollider;		// 回避用コライダー
	std::vector<Tank*> m_otherTanks;						// 自分以外の戦車
	std::unique_ptr<Smoke> m_smokeEffect;					// 倒されたときのエフェクト
	unsigned int m_maxHp;									// 最大体力
	unsigned int  m_hp;										// 体力
	bool m_isDead;											// 死亡しているかどうか
	Tank* m_targetTank;										// 攻撃してきた他の戦車
	bool m_isAvoidWall;										// 壁の回避


public:
	// 車体情報の設定
	void SetBody(TankBody* body) { m_body = body; }
	// 砲塔情報の設定
	void SetTurret(TankTurret* turret) { m_turret = turret; }
	// 砲身情報の設定
	void SetCannon(TankCannon* cannon) { m_cannon = cannon; }
	// 他の戦車情報の設定
	void SetOtherTanks(std::vector<Tank*> tanks) { m_otherTanks = tanks; }
	// 体力の設定
	void SetMaxHp(int hp) 
	{
		m_maxHp = hp; 
		m_hp = hp;
	}
	// 攻撃してきた敵の設定
	void SetTargetTank(Tank* targetTank) { m_targetTank = targetTank; }
	// 壁の回避をするのかの設定
	void SetAvoidWall(bool avoidWall) { m_isAvoidWall = avoidWall; }

	// 車体情報の取得
	TankBody* GetBody() { return m_body; }
	// 砲塔情報の取得
	TankTurret* GetTurret() { return m_turret; }
	// 砲身情報の取得
	TankCannon* GetCannon() { return m_cannon; }

	// 初期座標の取得
	DirectX::SimpleMath::Vector3 GetInitialPosition(){ return m_initialPosition; }
	// 戦車座標の取得
	DirectX::SimpleMath::Vector3 GetPosition() { return m_body->GetPosition(); }
	// 戦車の角度の取得
	DirectX::SimpleMath::Quaternion GetRotation() { return m_body->GetRotation(); }
	// 砲塔角度の取得
	DirectX::SimpleMath::Quaternion GetTurretRotation() { return m_turret->GetTurretRotation(); }
	// 砲身角度の取得
	DirectX::SimpleMath::Quaternion GetCannonRotation() { return m_cannon->GetCannonRotation(); }
	// コライダーの取得
	DirectX::BoundingBox* GetBoundingBox() { return m_collider->GetBoundingBox(); }
	// コライダーのポインタの取得
	BoxCollider* GetCollider() { return m_collider.get(); }
	// 回避用コライダーのポインタ取得
	SphereCollider* GetAvoidCollider() { return m_avoidCollider.get(); }
	// 戦車番号の取得
	int GetTankNumber() { return m_tankNumber; }
	// 体力の取得
	int GetHp() { return m_hp; }
	// 体力の減っている割合の取得
	float GetHpRatio() { return 1.0f - (static_cast<float>(m_hp) / static_cast<float>(m_maxHp)); }
	// 攻撃してきた敵の取得
	Tank* GetTargetTank() { return m_targetTank; }
	// 死亡しているかどうか
	bool GetDead() { return m_isDead; }
	// 壁を回避するかどうか
	bool GetAvoidWall() { return m_isAvoidWall; }

	// ダメージ処理
	void Damage(int damage);
};
#pragma once
#include "Interface/IParent.h"
#include "Interface/IParts.h"

#include "Game/Objects/Tank/TankBase/TankBody.h"
#include "Game/Objects/Tank/TankBase/TankTurret.h"
#include "Game/Objects/Tank/TankBase/TankCannon.h"

#include "Game/Collider/BoxCollider.h"

class TankBody;
class TankTurret;
class TankCannon;

class BoxCollider;

class Tank : public IParent
{
private:
	// コライダーのサイズ
	const DirectX::SimpleMath::Vector3 COLLIDER_SIZE = DirectX::SimpleMath::Vector3(1.2f, 0.7f, 1.2f);
	// コライダーを上にずらす
	const float COLLIDER_POSITION = 0.5f;
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
	std::vector<Tank*> m_otherTanks;						// 自分以外の戦車

public:
	// 車体情報の設定
	void SetBody(TankBody* body) { m_body = body; }
	// 砲塔情報の設定
	void SetTurret(TankTurret* turret) { m_turret = turret; }
	// 砲身情報の設定
	void SetCannon(TankCannon* cannon) { m_cannon = cannon; }
	// 他の戦車情報の設定
	void SetOtherTanks(std::vector<Tank*> tanks) { m_otherTanks = tanks; }

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
	// 戦車番号の取得
	int GetTankNumber() { return m_tankNumber; }

	// 当たり判定
	bool DetectCollisionTankAndNomalBullets();	// 戦車の通常弾の当たり判定
	bool DetectCollisionTankAndCannonBall();	// 戦車の通常弾の当たり判定
	void DetectCollisionTankAndOtherTanks();	// 戦車同士の当たり判定
};
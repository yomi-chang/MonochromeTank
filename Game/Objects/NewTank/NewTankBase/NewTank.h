#pragma once
#include "Interface/IObject.h"

#include "Game/Objects/NewTank/NewTankBase/NewTankBody.h"
#include "Game/Objects/NewTank/NewTankBase/NewTankTurret.h"
#include "Game/Objects/NewTank/NewTankBase/NewTankCannon.h"

class NewTankBody;
class NewTankTurret;
class NewTankCannon;

class NewTank : public IObject
{
public:
	// コンストラクタ
	NewTank(
		const DirectX::SimpleMath::Vector3& initialPosition,
		const float& initialAngle
	);
	// デストラクタ
	~NewTank() override;
	// 初期化処理
	void Initialize() override;
	// 更新処理
	void Update(float elapsedTime)override;
	void Update(
		float elapsedTime,
		const DirectX::SimpleMath::Vector3& currentPosition,
		const DirectX::SimpleMath::Quaternion& currentAngle
	) override {};
	// 描画処理
	void Render() override;
	// 終了処理
	void Finalize() override;

	// パーツ追加
	void Attach(std::unique_ptr<IObject> part) override;
	// パーツ削除
	void Detach(std::unique_ptr<IObject> part) override;

private:
	Graphics* m_graphics;									// グラフィックス
	DirectX::SimpleMath::Vector3 m_initialPosition;			// 初期座標
	DirectX::SimpleMath::Quaternion m_initialAngle;			// 初期回転角
	DirectX::SimpleMath::Vector3 m_currentPosition;			// 現在の座標
	DirectX::SimpleMath::Quaternion m_currentAngle;			// 現在の回転角
	std::vector<std::unique_ptr<IObject>> m_tankParts;		// 自身が管理する戦車部品の配列
	DirectX::SimpleMath::Matrix m_worldMatrix;				// ワールド行列

	// 各パーツ
	NewTankBody* m_body;
	NewTankTurret* m_turret;
	NewTankCannon* m_cannon;

	// 影描画用
	// 影用ポリゴン
	DirectX::VertexPositionTexture m_vertices[4];
	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch;
	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	//	エフェクト 
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

public:
	// 車体情報の受け取り
	void SetBody(NewTankBody* body) { m_body = body; }
	// 砲塔情報の受け取り
	void SetTurret(NewTankTurret* turret) { m_turret = turret; }
	// 砲身情報の受け取り
	void SetCannon(NewTankCannon* cannon) { m_cannon = cannon; }

	// 車体情報の受け取り
	NewTankBody* GetBody() { return m_body; }
	// 砲塔情報の受け取り
	NewTankTurret* GetTurret() { return m_turret; }
	// 砲身情報の受け取り
	NewTankCannon* GetCannon() { return m_cannon; }

	// 初期座標の受け取り
	DirectX::SimpleMath::Vector3 GetInitialPosition(){ return m_initialPosition; }

	// 戦車座標の受け取り
	DirectX::SimpleMath::Vector3 GetPosition() { return m_body->GetPosition(); }
	// 戦車の角度の受け取り
	DirectX::SimpleMath::Quaternion GetAngle() { return m_body->GetAngle(); }
	// 砲塔角度の受け取り
	DirectX::SimpleMath::Quaternion GetTurretAngle() { return m_turret->GetTurretAngle(); }
	// 砲身角度の受け取り
	DirectX::SimpleMath::Quaternion GetCannonAngle() { return m_cannon->GetCannonAngle(); }
	// 残り弾数の取得
};
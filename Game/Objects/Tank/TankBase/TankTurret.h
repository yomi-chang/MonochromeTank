#pragma once
#include "Interface/IParts.h"

class Tank;

class TankTurret : public IParts
{
public:
	// コンストラクタ
	TankTurret(
		Tank* tank,
		const DirectX::SimpleMath::Vector3& initialPosition,
		const float& initialAngle
	);
	// デストラクタ
	~TankTurret() override;
	// 初期化処理
	void Initialize() override;
	// 更新処理
	void Update(
		float elapsedTime,
		const DirectX::SimpleMath::Vector3& currentPosition,
		const DirectX::SimpleMath::Quaternion& currentRotation
	) override;
	// 自身を描画しない描画処理
	void Render() override;
	// 終了処理
	void Finalize() override;

	// パーツ追加
	void Attach(std::unique_ptr<IParts> parts) override;
	// パーツ削除
	void Detach(std::unique_ptr<IParts> parts) override;

private:
	Graphics* m_graphics;								// グラフィックス				
	DirectX::SimpleMath::Vector3 m_initialPosition;		// 初期座標	
	DirectX::SimpleMath::Quaternion m_initialRotation;	// 初期回転角	
	DirectX::SimpleMath::Vector3 m_currentPosition;		// 現在の座標	
	DirectX::SimpleMath::Quaternion m_currentRotation;	// 現在の回転角	
	std::vector<std::unique_ptr<IParts>> m_tankParts;	// 自身が管理する戦車部品の配列	
	DirectX::SimpleMath::Matrix m_worldMatrix;			// ワールド行列	
	DirectX::Model* m_model;							// モデル	

	DirectX::SimpleMath::Quaternion m_turretRotation;		// 砲塔の角度

	Tank* m_tank;

public:
	// 砲塔の回転
	void RotateTurret(float angle);

	// 砲塔の角度の取得
	DirectX::SimpleMath::Quaternion GetTurretRotation() { return m_turretRotation; }
};
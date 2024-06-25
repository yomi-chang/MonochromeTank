// 戦車の更新や描画などを担うクラス
#pragma once
#include "Interface/ITankComponent.h"
#include "Game/Objects/Tank/TankBase.h"
#include "Game/Objects/Tank/Tank.h"

class TankCannon : public TankBase
{
	// インターバル
	const float SHOT_INTERVAL = 0.2f;

	// 砲身の角度の制限
	const float CANON_ANGLEUD_MIN = DirectX::XMConvertToRadians(-10.0f);
	const float CANON_ANGLEUD_MAX = DirectX::XMConvertToRadians(10.0f);

public:
	float GetCannonAngle() { return m_currentAngleUD; }

public:
	// コンストラクタ
	TankCannon(
		ITankComponent* parent,
		const DirectX::SimpleMath::Vector3& initialPosition,
		const float& initialAngleRL,
		TankType type
	);

	// デストラクタ
	~TankCannon() override;

	// 初期化処理
	void Initialize() override;

	// 更新処理
	void Update(
		float elapsedTime,
		const DirectX::SimpleMath::Vector3& currentPosition,
		const float& currentAngleRL
	) override;

	// 自身を描画しない描画処理
	void Render() override;

	// 終了処理
	void Finalize() override;

	// 砲弾を発射する
	void Shoot(IBullet* bullet);

private:
	// グラフィックス
	Graphics* m_graphics;

	// 初期位置
	DirectX::SimpleMath::Vector3 m_currentPosition;

	// 初期回転角
	float m_currentAngleRL;

	// 自身が管理する戦車部品の配列
	std::vector<std::unique_ptr<ITankComponent>> m_tankParts;

	// モデル
	DirectX::Model* m_model;

	// ワールド行列
	DirectX::SimpleMath::Matrix m_worldMatrix;

	// 上下の回転角
	float m_currentAngleUD;

	// 砲身の角度
	float m_cannonAngle;

	// 使用済み砲弾数
	int m_shotBulletNumber;

	// 砲弾発射タイマー
	float m_shotTimer;

	// 砲塔
	Tank* m_tank;

	// 敵かプレイヤーか
	TankBase::TankType m_tankType;
};
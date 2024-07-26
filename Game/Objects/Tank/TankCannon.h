// 戦車の更新や描画などを担うクラス
#pragma once
#include "Interface/IComponent.h"
#include "Game/Objects/Tank/TankBase.h"
#include "Game/Objects/Tank/Tank.h"

#include "Interface/ILeaf.h"

class TankCannon : public ILeaf
{
	// インターバル
	const float SHOT_INTERVAL = 0.2f;

	// 砲身の角度の制限
	const float CANON_ANGLEUD_MIN = DirectX::XMConvertToRadians(-10.0f);
	const float CANON_ANGLEUD_MAX = DirectX::XMConvertToRadians(10.0f);

public:
	// 親オブジェクトを取得する
	IComponent* GetParent() const { return m_parent; }

	// 銃口の座標を取得する
	DirectX::SimpleMath::Vector3 GetMuzzlePosition();

public:
	// コンストラクタ
	TankCannon(
		IComponent* parent,
		const DirectX::SimpleMath::Vector3& initialPosition,
		const float& initialAngleRL
	);

	// デストラクタ
	~TankCannon();

	// 初期化処理
	void Initialize(Type type);

	// 更新処理
	void Update(
		float elapsedTime,
		const DirectX::SimpleMath::Vector3& currentPosition,
		const float& currentAngleRL
	);

	// 自身を描画しない描画処理
	void Render();

	// 終了処理
	void Finalize();

	// 砲弾を発射する
	void Shoot(IBullet* bullet);

private:
	// グラフィックス
	Graphics* m_graphics;

	// 親オブジェクト
	IComponent* m_parent;

	// 初期座標
	DirectX::SimpleMath::Vector3 m_initialPosition;

	// 初期回転角
	float m_initialAngle;

	// 現在の座標
	DirectX::SimpleMath::Vector3 m_currentPosition;

	// 現在の回転角
	float m_currentAngleRL;

	// 自身が管理する戦車部品の配列
	std::vector<std::unique_ptr<IComponent>> m_tankParts;

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
	Type m_tankType;
};
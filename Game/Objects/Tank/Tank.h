#pragma once
#include "Interface/ITankComponent.h"
#include "Game/Objects/Tank/TankBase.h"
#include "Interface/IBullet.h"
#include "Game/Objects/Bullet/Bullet.h"

class Tank : public TankBase
{
public:
	// 戦車座標の取得
	DirectX::SimpleMath::Vector3 GetTankPosition() { return m_currentPosition; }
	// 戦車の向きの取得
	float GetTankAngleRL() { return m_currentAngleRL; }
	// 「砲弾」を参照する
	std::vector<std::unique_ptr<IBullet>>& GetBullets() { return m_bullets; };

	// 他戦車の情報の受け取り
	void SetOtherTank(Tank* tank) { m_otherTank = tank; }

public:
	// コンストラクタ
	Tank(
		ITankComponent* parent,
		const DirectX::SimpleMath::Vector3& initialPosition,
		const float& initialAngleRL,
		TankType type
	);

	// デストラクタ
	~Tank() override;

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

private:
	// グラフィックス
	Graphics* m_graphics;

	// 現在の位置
	DirectX::SimpleMath::Vector3 m_currentPosition;

	// 現在の回転角
	float m_currentAngleRL;

	// 自身が管理する戦車部品の配列
	std::vector<std::unique_ptr<ITankComponent>> m_tankParts;

	// ワールド行列
	DirectX::SimpleMath::Matrix m_worldMatrix;

	// 砲弾配列
	std::vector<std::unique_ptr<IBullet>> m_bullets;

	// 敵かプレイヤーか
	TankBase::TankType m_tankType;

	// 他戦車の情報
	Tank* m_otherTank;

private:
	void PlayerAction();
	void EnemyAction();
};
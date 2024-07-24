#pragma once
#include "Interface/IComponent.h"
#include "Game/Objects/Tank/TankBase.h"
#include "Interface/IBullet.h"
#include "Game/Objects/Bullet/Bullet.h"
#include "Game/Collider/SphereCollider.h"
#include "Interface/IComposite.h"
#include "Game/UserInterface/HpGauge.h"

class Tank : public IComposite
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

	// 当たっているかどうか
	bool GetHit() { return m_hit; }

	// コライダーの取得
	DirectX::BoundingSphere* GetBoundingSphere() { return m_collider->GetBoundingSphere(); }

	// 親オブジェクトを取得する
	IComponent* GetParent() const { return m_parent; }

public:
	// コンストラクタ
	Tank(
		IComponent* parent,
		const DirectX::SimpleMath::Vector3& initialPosition,
		const float& initialAngleRL
	);

	// デストラクタ
	~Tank() override;

	// 初期化処理
	void Initialize(Type type) override;

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

	// 部品を追加する
	void Attach(std::unique_ptr<IComponent> part);

	// 部品を削除する
	void Detach(std::unique_ptr<IComponent> part);

private:
	// グラフィックス
	Graphics* m_graphics;
	
	// 親オブジェクト
	IComponent* m_parent;

	// 初期座標
	DirectX::SimpleMath::Vector3 m_initialPosition;

	// 初期回転角
	float m_initialAngle;

	// 現在の位置
	DirectX::SimpleMath::Vector3 m_currentPosition;

	// 現在の回転角
	float m_currentAngleRL;

	// 自身が管理する戦車部品の配列
	std::vector<std::unique_ptr<IComponent>> m_tankParts;

	// ワールド行列
	DirectX::SimpleMath::Matrix m_worldMatrix;

	// 砲弾配列
	std::vector<std::unique_ptr<IBullet>> m_bullets;

	// 敵かプレイヤーか
	Type m_tankType;

	// 他戦車の情報
	Tank* m_otherTank;

	// 当たり判定
	std::unique_ptr<SphereCollider> m_collider;

	// ヒットしているかどうか
	bool m_hit;

	// 体力
	int m_hpValue;

	// 体力ゲージ
	std::unique_ptr<HpGauge> m_hpGauge;

private:
	// プレイヤーの行動
	void PlayerAction();
	// 敵行動
	void EnemyAction();

	// 戦車と砲弾の衝突判定を行う
	void DetectCollisionTankAndBullets();

	// 戦車と戦車の衝突判定を行う
	void DetectCollisionTankAndOtherTanks();
};
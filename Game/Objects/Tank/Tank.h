#pragma once
#include "Interface/IComponent.h"
#include "Interface/IBullet.h"
#include "Game/Objects/Bullet/Bullet.h"
#include "Game/Objects/Bullet/CannonBall.h"
#include "Game/Collider/SphereCollider.h"
#include "Interface/IComposite.h"
#include "Game/UserInterface/HpGauge.h"
#include "Game/UserInterface/EnemyHpGauge.h"

class Tank : public IComposite
{
public:
	// 弾の種類
	enum BulletType
	{
		BULLET,			// 連射弾
		CANNONBALL		// 砲弾
	};

public:
	// 戦車座標の取得
	DirectX::SimpleMath::Vector3 GetTankPosition() { return m_currentPosition; }
	// 戦車座標を入れる
	void SetTankPosition(DirectX::SimpleMath::Vector3 position) { m_currentPosition = position; }

	// 戦車の向きの取得
	float GetTankAngleRL() { return m_currentAngleRL; }

	// 「連射弾」を参照する
	std::vector<std::unique_ptr<IBullet>>& GetBullets() { return m_bullets; };
	
	// 「砲弾」を参照する
	std::unique_ptr<IBullet>& GetCannonBall() { return m_cannonBall; }

	// 他戦車の情報の受け取り
	//void SetOtherTank(std::vector<Tank*> tank) { m_otherTanks = tank; }

	// プレイヤー情報の受取り(敵側)
	void SetOtherTank(Tank* tank) { m_otherTanks.push_back(tank); }

	// 当たっているかどうか
	bool GetHit() { return m_hit; }

	// コライダーの取得
	DirectX::BoundingSphere* GetBoundingSphere() { return m_collider->GetBoundingSphere(); }

	// 親オブジェクトを取得する
	IComponent* GetParent() const { return m_parent; }

	// 体力を渡す
	int GetHpValue() { return m_hpValue; }

	// 残弾数を渡す
	int GetBulletValue() 
	{
		int value = 0;
		// 使われていない弾の数を調べる
		for (auto& bullet : m_bullets)
		{
			if (bullet->GetBulletState() == IBullet::UNUSED)
				value++;
		}
		return value;
	}

	int GetCannonBallValue() 
	{
		// 弾の使用未使用によって変更
		if (m_cannonBall->GetBulletState() == IBullet::UNUSED)
		{
			return 1;
		}
		return 0;
	}
	
	// 現在選択されている弾の種類を渡す
	BulletType GetBulletType() { return m_bulletType; }

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
	// 現在の座標
	DirectX::SimpleMath::Vector3 m_currentPosition;
	// 現在の回転角
	float m_currentAngleRL;
	// 自身が管理する戦車部品の配列
	std::vector<std::unique_ptr<IComponent>> m_tankParts;
	// ワールド行列
	DirectX::SimpleMath::Matrix m_worldMatrix;
	// 敵かプレイヤーか
	Type m_tankType;

private:
	// 砲弾配列
	std::vector<std::unique_ptr<IBullet>> m_bullets;
	std::unique_ptr<IBullet> m_cannonBall;
	// 他戦車の情報
	std::vector<Tank*> m_otherTanks;
	// 当たり判定
	std::unique_ptr<SphereCollider> m_collider;
	// ヒットしているかどうか
	bool m_hit;

	// 体力
	int m_hpValue;

	// 体力ゲージ
	std::unique_ptr<HpGauge> m_hpGauge;

	// 敵体力ゲージ
	std::unique_ptr<EnemyHpGauge> m_enemyHpGauge;

	// 現在の弾の種類
	BulletType m_bulletType;

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
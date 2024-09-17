#pragma once
#include "Interface/IComponent.h"
#include "Interface/IBullet.h"
#include "Game/Objects/Bullet/Bullet.h"
#include "Game/Objects/Bullet/CannonBall.h"
#include "Game/Collider/SphereCollider.h"
#include "Interface/IComposite.h"
#include "Game/UserInterface/HpGauge.h"
#include "Game/UserInterface/EnemyHpGauge.h"

#include "Game/Objects/Stage/Wall.h"
#include "Libraries/MyLib/FollowCamera.h"

namespace mylib
{
	class FollowCamera;
}
class Wall;

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

	
	//float GetTankAngleRL() { return m_currentAngleRL; }
	
	// 戦車の向きの取得
	DirectX::SimpleMath::Quaternion GetTankAngle() { return m_currentAngle; }

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

	// 死亡しているかどうか
	bool GetDead();
	

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

	// カメラ情報の受け取り
	void SetCamera(mylib::FollowCamera* camera) { m_camera = camera; }
	// カメラの振動
	void ShakeCamera() { m_camera->StartShakeCamera(); }

	// 壁情報の受け取り
	void SetWalls(std::vector<Wall*> walls)
	{
		for (Wall* wall : walls)
		{
			m_walls.push_back(wall);
		}
	}
	// 壁情報の取得
	std::vector<Wall*> GetWalls() { return m_walls; }

public:
	// コンストラクタ
	Tank(
		IComponent* parent,
		const DirectX::SimpleMath::Vector3& initialPosition,
		const float& initialAngle
	);

	// デストラクタ
	~Tank() override;

	// 初期化処理
	void Initialize(Type type) override;

	// 更新処理
	void Update(
		float elapsedTime,
		const DirectX::SimpleMath::Vector3& currentPosition,
		const DirectX::SimpleMath::Quaternion& currentAngle
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
	DirectX::SimpleMath::Quaternion m_initialAngle;
	// 現在の座標
	DirectX::SimpleMath::Vector3 m_currentPosition;
	// 現在の回転角
	DirectX::SimpleMath::Quaternion m_currentAngle;
	// 自身が管理する戦車部品の配列
	std::vector<std::unique_ptr<IComponent>> m_tankParts;
	// ワールド行列
	DirectX::SimpleMath::Matrix m_worldMatrix;
	// 敵かプレイヤーか
	Type m_tankType;

	// カメラ
	mylib::FollowCamera* m_camera;

	// 壁
	std::vector<Wall*> m_walls;

private:
	// 砲弾配列
	std::vector<std::unique_ptr<IBullet>> m_bullets;	// 連射弾
	std::unique_ptr<IBullet> m_cannonBall;				// 砲弾
	// 他戦車の情報
	std::vector<Tank*> m_otherTanks;
	// 当たり判定
	std::unique_ptr<SphereCollider> m_collider;
	// ヒットしているかどうか
	bool m_hit;

	// ダメージ
	float m_damage;

	// 体力ゲージ
	std::unique_ptr<HpGauge> m_hpGauge;

	// 敵体力ゲージ
	std::unique_ptr<EnemyHpGauge> m_enemyHpGauge;

	// 現在の弾の種類
	BulletType m_bulletType;

	// リロードカウント
	const float BULLET_RELOAD_TIME = 1.0f;
	const float CANNONBALL_RELOAD_TIME = 1.0f;
	float m_reloadCount;
	bool m_isReload;
	// リロードしている弾の種類
	BulletType m_reloadBulletType;


	//mylib::FollowCamera* m_camera;

private:
	// プレイヤーの行動
	void PlayerAction(float elapsedTime);
	// 敵行動
	void EnemyAction();
	// 戦車と砲弾の衝突判定を行う
	void DetectCollisionTankAndBullets();
	// 戦車と戦車の衝突判定を行う
	void DetectCollisionTankAndOtherTanks();
};
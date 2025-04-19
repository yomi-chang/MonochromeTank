/*
	@file	Parameter.h
	@brief	パラメータークラス
*/
#pragma once

class Parameter
{
public:
	// Parameterクラスのインスタンスを取得する
	static Parameter* const GetInstance();

public:
	// デストラクタ
	~Parameter();

private:
	// コンストラクタ
	Parameter();
	// 代入は許容しない
	void operator=(const Parameter& object) = delete;
	// コピーコンストラクタは許容しない
	Parameter(const Parameter& object) = delete;

private:
	// 戦車の回転制限
	float m_turretAngleMin;										// 砲塔の角度制限
	float m_turretAngleMax;
	float m_cannonAngleMin;										// 砲身の角度制限
	float m_cannonAngleMax;
	// 自機
	DirectX::SimpleMath::Vector3 m_playerPosition;				// 自機の初期座標
	float m_playerSpeed;										// 自機の移動速度
	float m_playerRotationSpeed;								// 自機の回転速度
	int   m_playerHp;											// 自機の体力
	// 敵戦車
	std::vector<DirectX::SimpleMath::Vector3> m_enemyPositions;	// 敵戦車座標
	float m_enemySpeed;											// 敵戦車の移動速度
	float m_enemyRotationSpeed;									// 敵戦車の回転速度
	int   m_enemyHp;											// 敵戦車の体力
	// 弾
	float m_shotInterval;										// 射撃のインターバル
	// 連射弾
	int   m_bulletCount;										// 弾数
	float m_bulletReloadTime;									// リロード時間
	float m_bulletSurvivalTime;									// 生存時間
	DirectX::SimpleMath::Vector3 m_bulletSpeed;					// 速度
	float m_bulletColliderRadius;								// コライダーの大きさ
	int   m_bulletDamage;										// ダメージ
	int   m_bulletMaxTrail;										// トレイルの長さ
	float m_bulletTrailWidth;									// トレイルの幅
	// 砲弾
	float m_cannonBallReloadTime;								// リロード時間
	float m_cannonBallSurvivalTime;								// 生存時間
	DirectX::SimpleMath::Vector3 m_cannonBallSpeed;				// 速度
	DirectX::SimpleMath::Vector3 m_cannonBallGravity;			// 重力
	float m_cannonBallColliderRadius;							// コライダー大きさ
	int   m_cannonBallDamage;									// ダメージ
	int   m_cannonBallMaxTrail;										// トレイルの長さ
	float m_cannonBallTrailWidth;									// トレイルの幅
	// 壁サイズ
	DirectX::SimpleMath::Vector3 m_wallSize;					// 壁サイズ
	// 索敵範囲
	float m_scoutRadius;		
	// 攻撃開始範囲
	float m_attackStartRadius;
	// 攻撃終了範囲
	float m_attackFinishRadius;
	// フェード速度
	float m_fadeSpeed;
	// 照準の射程距離
	float m_maxRange;
	// ダメージの演出時間
	float m_damageEffectTime;
	// 巡回地点
	using PatrolRoute = std::vector<DirectX::SimpleMath::Vector3>;
	std::vector<PatrolRoute> m_patrolRoutes;

public:
	// パラメータの読み込み
	void LoadParameter();

	// 戦車の回転制限
	float GetTurretAngleMin() { return m_turretAngleMin; }
	float GetTurretAngleMax() { return m_turretAngleMax; }
	float GetCannonAngleMin() { return m_cannonAngleMin; }
	float GetCannonAngleMax() { return m_cannonAngleMax; }
	// 自機
	const DirectX::SimpleMath::Vector3& GetPlayerPosition() { return m_playerPosition; }
	float GetPlayerSpeed() { return m_playerSpeed; }
	float GetPlayerRotationSpeed() { return m_playerRotationSpeed; }
	int   GetPlayerHp() { return m_playerHp; }
	// 弾
	float GetShotInterval() { return m_shotInterval; }
	// 敵戦車
	const DirectX::SimpleMath::Vector3& GetEnemyPosition(int number) { return m_enemyPositions.at(number); }
	float GetEnemySpeed() { return m_enemySpeed; }
	float GetEnemyRotationSpeed() { return m_enemyRotationSpeed; }
	int   GetEnemyHp() { return m_enemyHp; }
	// 連射弾
	int   GetBulletCount() { return m_bulletCount; }
	float GetBulletReloadTime() { return m_bulletReloadTime; }
	float GetBulletSurvivalTime() { return m_bulletSurvivalTime; }
	const DirectX::SimpleMath::Vector3& GetBulletSpeed() { return m_bulletSpeed; }
	float GetBulletColliderRadius() { return m_bulletColliderRadius; }
	int   GetBulletDamage() { return m_bulletDamage; }
	int   GetBulletMaxTrail() { return m_bulletMaxTrail; }
	float GetBulletWidth() { return m_bulletTrailWidth; }
	// 砲弾
	float GetCannonBallReloadTime() { return m_cannonBallReloadTime; }
	float GetCannonBallSurvivalTime() { return m_cannonBallSurvivalTime; }
	const DirectX::SimpleMath::Vector3& GetCannonBallSpeed() { return m_cannonBallSpeed; }
	const DirectX::SimpleMath::Vector3& GetCannonBallGravity() { return m_cannonBallGravity; }
	float GetCannonBallColliderRadius() { return m_cannonBallColliderRadius; }
	int   GetCannonBallDamage() { return m_cannonBallDamage; }
	int   GetCannonBallMaxTrail() { return m_cannonBallMaxTrail; }
	float GetCannonBallWidth() { return m_cannonBallTrailWidth; }
	// 壁サイズ
	const DirectX::SimpleMath::Vector3& GetWallSize() { return m_wallSize; }
	// 索敵範囲
	float GetScoutRadius() { return m_scoutRadius; }
	// 攻撃開始範囲
	float GetAttackStartRadius() { return m_attackStartRadius; }
	// 攻撃終了範囲
	float GetAttackFinishRadius() { return m_attackFinishRadius; }
	// フェード速度
	float GetFadeSpeed() { return m_fadeSpeed; }
	// 照準の射程距離
	float GetMaxRange() { return m_maxRange; }
	// ダメージの演出時間
	float GetDamageEffectTime() { return m_damageEffectTime; }
	// 巡回ルートの数の取得
	int GetPatrolRouteSize() { return static_cast<int>(m_patrolRoutes.size()); }
	// 巡回ルートの取得
	std::vector<DirectX::SimpleMath::Vector3> GetPatrolRoute(int routeNumber) { return m_patrolRoutes.at(routeNumber); }

private:
	// Parameterクラスのインスタンスへのポインタ
	static std::unique_ptr<Parameter> m_parameter;	
};


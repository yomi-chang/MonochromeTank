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
	float m_turretAngleMin;
	float m_turretAngleMax;
	float m_cannonAngleMin;
	float m_cannonAngleMax;

	// 自機
	float m_playerSpeed;
	float m_playerRotationSpeed;

	// 敵戦車の速度
	float m_enemySpeed;
	float m_enemyRotationSpeed;


	// 自機
	//static constexpr float PLAYER_SPEED = 3.0f;
	//static constexpr float PLAYER_ROTATION_SPEED = 0.75f;

	// 敵戦車の速度
	//static constexpr float ENEMY_SPEED = 2.0f;
	
public:
	// 連射弾の弾数
	static constexpr int BULLET_COUNT = 30;

	// 各種弾のリロード時間
	static constexpr float BULLET_RELOAD_TIME = 1.0f;
	static constexpr float CANNONBALL_RELOAD_TIME = 2.0f;

	// 射撃インターバル
	static constexpr float SHOT_INTERVAL = 0.15f;
	static constexpr float ENEMY_SHOT_INTERVAL = 1.0f;

	// 照準の射程距離
	static constexpr float MAX_RANGE = 4.0f;

public:
	// パラメータの読み込み
	void LoadParameter();

	// 戦車の回転制限の取得
	float GetTurretAngleMin() { return m_turretAngleMin; }
	float GetTurretAngleMax() { return m_turretAngleMax; }
	float GetCannonAngleMin() { return m_cannonAngleMin; }
	float GetCannonAngleMax() { return m_cannonAngleMax; }
	// プレイヤーの速度の取得
	float GetPlayerSpeed() { return m_playerSpeed; }
	// プレイヤーの回転速度の取得
	float GetPlayerRotationSpeed() { return m_playerRotationSpeed; }
	// 敵戦車の速度の取得
	float GetEnemySpeed() { return m_enemySpeed; }
	// 敵戦車の回転速度の取得
	float GetEnemyRotationSpeed() { return m_enemyRotationSpeed; }

	
private:
	// Parameterクラスのインスタンスへのポインタ
	static std::unique_ptr<Parameter> m_parameter;

	

	// マウス感度倍率
	//static constexpr float


	//// パラメータ
	//// 連射弾
	//DirectX::SimpleMath::Vector3 BULLET_SPEED = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -0.3f);
	//float BULLET_SURVIVAL_TIME = 1.5f;

	//// 砲弾
	//DirectX::SimpleMath::Vector3 CANNONBALL_GRAVITY = DirectX::SimpleMath::Vector3(0.0f, -0.05f, 0.0f);
	//DirectX::SimpleMath::Vector3 CANNONBALL_SPEED = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -0.12f);

	//// ステージ
	//int STAGE_SIZE;
	//DirectX::SimpleMath::Vector3 WALL_SIZE;

	//// 戦車
	//DirectX::SimpleMath::Vector3 COLLIDER_SIZE;
	//float DEFAULT_HP;

	//// 敵戦車
	//float ENEMY_TANK_SPEED;
	//float SEARCH_DISTANCE;

	//// 戦車の数
	//int TankCount;
	//
	//DirectX::SimpleMath::Vector3 TankPosition;
	
};


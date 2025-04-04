/*
	@file	FixedTurret.h
	@brief	固定砲台クラス
*/
#pragma once

class Tank;
class IBullet;

class FixedTurret
{
public:
	FixedTurret(DirectX::SimpleMath::Vector3 position);
	~FixedTurret() = default;
	void Initialize();
	void Update(float elapsedTime);
	void Render();

private:
	// 弾の発射インターバル
	static constexpr float SHOT_INTERVAL = 0.3f;
	// リロード時間
	static constexpr float RELOAD_TIME = 2.0f;
	// 弾数
	static constexpr int BULLET_CAPACITY = 500;
	// 射程距離
	static constexpr float MAX_RANGE = 15.0f;
	// 回転速度
	static constexpr float ROTATION_SPEED = 0.9f;


	DirectX::SimpleMath::Vector3 m_position;			// 座標
	DirectX::SimpleMath::Quaternion m_angle;			// 角度

	std::vector<Tank*> m_tanks;							// 全戦車情報
	Tank* m_targetTank;									// 追跡対象の戦車

	std::vector<std::unique_ptr<IBullet>> m_bullets;	// 連射弾

	DirectX::Model* m_model;							// モデル

	float m_shotTimer;									// 砲弾発射タイマー
	float m_reloadCount;								// 弾のリロードカウント
	float m_isReload;									// リロード中かどうか

public:
	// 戦車情報の設定
	void SetTanks(std::vector<Tank*> tanks) { m_tanks = tanks; }

	// 弾情報の取得
	std::vector<std::unique_ptr<IBullet>>& GetBullets() { return m_bullets; };

private:
	// 砲弾を発射する
	void ShootBullet(IBullet* bullet);

	// 銃口の座標を取得する
	DirectX::SimpleMath::Vector3 GetMuzzlePosition();

	// 発射処理
	void Shot();

	// リロード処理
	void Reload(float elapsedTime);

	void StartReload();

private:
	void ChangeTaegetTank();
};
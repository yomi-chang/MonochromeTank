#pragma once

class Tank;
class IBullet;

class FixedTurret
{
public:
	FixedTurret();
	~FixedTurret() = default;
	void Initialize();
	void Update(float elapsedTime);
	void Render();

private:
	// 弾の発射インターバル
	const float SHOT_INTERVAL = 0.3f;
	// リロード時間
	const float RELOAD_TIME = 2.0f;
	// 弾数
	const int BULLET_CAPACITY = 20;

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
	// デバッグ用モデル
	std::unique_ptr<DirectX::GeometricPrimitive> m_box;
};
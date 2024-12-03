#pragma once
#include "Interface/IObject.h"
#include "Interface/IBullet.h"

class Tank;
class Wall;
class DrawTexture;

class TankCannon : public IObject
{
public:
	// 弾の種類
	enum BulletType
	{
		BULLET,			// 連射弾
		CANNONBALL		// 砲弾
	};

public:
	// 各種弾におけるリロード時間
	const float BULLET_RELOAD_TIME = 1.0f;
	const float CANNONBALL_RELOAD_TIME = 1.0f;

	// インターバル
	const float SHOT_INTERVAL = 0.2f;
	const float ENEMY_SHOT_INTERVAL = 1.0f;

	// 照準の射程距離
	const float MAX_RANGE = 5.0f;
public:
	// コンストラクタ
	TankCannon(
		Tank* tank,
		const DirectX::SimpleMath::Vector3& initialPosition,
		const float& initialAngle
	);
	// デストラクタ
	~TankCannon() override;
	// 初期化処理
	void Initialize() override;
	// 更新処理
	void Update(float elapsedTime)override{}
	void Update(
		float elapsedTime,
		const DirectX::SimpleMath::Vector3& currentPosition,
		const DirectX::SimpleMath::Quaternion& currentAngle
	) override;
	// 自身を描画しない描画処理
	void Render() override;
	// 終了処理
	void Finalize() override;

	// パーツ追加
	void Attach(std::unique_ptr<IObject> part) override {};
	// パーツ削除
	void Detach(std::unique_ptr<IObject> part) override {};

private:
	Graphics* m_graphics;								// グラフィックス						
	DirectX::SimpleMath::Vector3 m_initialPosition;		// 初期座標
	DirectX::SimpleMath::Quaternion m_initialAngle;		// 初期回転角
	DirectX::SimpleMath::Vector3 m_currentPosition;		// 現在の座標
	DirectX::SimpleMath::Quaternion m_currentAngle;		// 現在の回転角
	std::vector<std::unique_ptr<IObject>> m_tankParts;	// 自身が管理する戦車部品の配列
	DirectX::SimpleMath::Matrix m_worldMatrix;			// ワールド行列
	DirectX::Model* m_model;							// モデル

	DirectX::SimpleMath::Quaternion m_cannonAngle;		// 砲身の角度

	std::vector<std::unique_ptr<IBullet>> m_bullets;	// 連射弾
	std::unique_ptr<IBullet> m_cannonBall;				// 砲弾
	BulletType m_bulletType;							// 現在の弾の種類
	float m_reloadCount;								// リロードカウント
	bool m_isReload;									// リロードしているか
	BulletType m_reloadBulletType;						// リロードしている弾の種類
	float m_shotTimer;									// 砲弾発射タイマー
	
	std::unique_ptr<DrawTexture> m_drawTexture;			// 画像の描画
	std::vector<Wall*> m_walls;							// 壁

	Tank* m_tank;									// 戦車情報


public:
	// 砲身の角度の取得
	DirectX::SimpleMath::Quaternion GetCannonAngle() { return m_cannonAngle; }

	// 砲身の回転
	void RotateCannon(float angle);

	// 砲弾を発射する
	void ShootBullet(IBullet* bullet);

	// 発射
	void Shoot();

	// 発射する弾の変更
	void ChangeBullet();

	// リロード
	void StartReload();

	// 壁情報の受け取り
	void SetWalls(std::vector<Wall*> walls)
	{
		for (Wall* wall : walls)
		{
			m_walls.emplace_back(wall);
		}
	}

	// 「連射弾」を参照する
	std::vector<std::unique_ptr<IBullet>>& GetBullets() { return m_bullets; };

	// 「砲弾」を参照する
	std::unique_ptr<IBullet>& GetCannonBall() { return m_cannonBall; }

	// 現在の弾情報を渡す
	BulletType GetCurrentBullet() { return m_bulletType; }

private:
	// 照準の表示
	void DisplaySight();

	// 銃口の座標を取得する
	DirectX::SimpleMath::Vector3 GetMuzzlePosition();

	// リロード処理
	void Reload(float elapsedTime);

private:
	// デバッグ用モデル
	std::unique_ptr<DirectX::GeometricPrimitive> m_box;
};
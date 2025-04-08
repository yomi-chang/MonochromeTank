/*
	@file	TankCannon.h
	@brief	砲身クラス
*/
#pragma once
#include "Interface/IParts.h"
#include "Interface/IBullet.h"
#include "Game/Other/Parameter.h"

class Tank;
class Wall;
class DrawTexture;

class TankCannon : public IParts
{
private:
	// マウス感度倍率
	static constexpr float MOUSE_SENCIBITY = 0.001f;

public:
	// 弾の種類
	enum BulletType
	{
		BULLET,			// 連射弾
		CANNONBALL		// 砲弾
	};

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
	void Update(
		float elapsedTime,
		const DirectX::SimpleMath::Vector3& currentPosition,
		const DirectX::SimpleMath::Quaternion& currentRotation
	) override;
	// 自身を描画しない描画処理
	void Render() override;
	// 終了処理
	void Finalize() override;

	// パーツ追加
	void Attach(std::unique_ptr<IParts> parts) override {};
	// パーツ削除
	void Detach(std::unique_ptr<IParts> parts) override {};

private:
	Graphics* m_graphics;								// グラフィックス						
	DirectX::SimpleMath::Vector3 m_initialPosition;		// 初期座標
	DirectX::SimpleMath::Quaternion m_initialRotation;	// 初期回転角
	DirectX::SimpleMath::Vector3 m_currentPosition;		// 現在の座標
	DirectX::SimpleMath::Quaternion m_currentRotation;	// 現在の回転角
	std::vector<std::unique_ptr<IParts>> m_tankParts;	// 自身が管理する戦車部品の配列
	DirectX::SimpleMath::Matrix m_worldMatrix;			// ワールド行列
	DirectX::Model* m_model;							// モデル

	DirectX::SimpleMath::Quaternion m_cannonRotation;	// 砲身の角度

	std::vector<std::unique_ptr<IBullet>> m_bullets;	// 連射弾
	std::unique_ptr<IBullet> m_cannonBall;				// 砲弾
	BulletType m_bulletType;							// 現在の弾の種類
	float m_reloadCount;								// リロードカウント
	bool m_isReload;									// リロードしているか
	BulletType m_reloadBulletType;						// リロードしている弾の種類
	float m_shotTimer;									// 砲弾発射タイマー
	float m_bulletBlurRadius;							// 弾のブレの半径
	bool m_isShot;										// 弾を発射しているかどうか
	
	std::unique_ptr<DrawTexture> m_drawTexture;			// 画像の描画

	Tank* m_tank;										// 戦車情報
	DirectX::SimpleMath::Vector3 m_hitPosition;			// Rayが当たった座標

	bool m_displaySight;								// 標準を表示するかどうか

	
public:
	// 砲身の角度の取得
	DirectX::SimpleMath::Quaternion GetCannonRotation() { return m_cannonRotation; }

	// 砲身の回転
	void RotateCannon(float angle);

	// 砲弾を発射する
	void ShootBullet(IBullet* bullet);

	// 発射
	void Shoot();
	// 発射終了
	void FinishShoot() { m_isShot = false; }

	// 発射する弾の変更
	void ChangeBullet();

	// リロード開始
	void StartReload();

	// Rayが壁と当たったかどうかと当たった座標を設定
	void SetRayInfo(bool isHit, DirectX::SimpleMath::Vector3 hitPosition);

	// 「連射弾」を参照する
	std::vector<std::unique_ptr<IBullet>>& GetBullets() { return m_bullets; };

	// 「砲弾」を参照する
	std::unique_ptr<IBullet>& GetCannonBall() { return m_cannonBall; }

	// 現在の弾情報を渡す
	BulletType GetCurrentBullet() { return m_bulletType; }

	// リロードがどれだけ完了しているかの割合を渡す
	float GetCannonReloadRatio() { return 1.0f - (m_reloadCount / Parameter::GetInstance()->GetCannonBallReloadTime()); }
	float GetBulletReloadRatio() { return 1.0f - (m_reloadCount / Parameter::GetInstance()->GetBulletReloadTime()); }

	// 銃口の座標を取得する
	DirectX::SimpleMath::Vector3 GetMuzzlePosition();

	// Rayの射程の取得
	float GetMaxRange() { return Parameter::GetInstance()->GetMaxRange(); }

	// 発射方向の取得
	DirectX::SimpleMath::Quaternion GetMuzzleRotation() { return m_cannonRotation * m_currentRotation; }

	// 照準を表示するかどうかの設定
	void SetDisplaySight(bool display) { m_displaySight = display; }

private:
	// リロード処理
	void Reload(float elapsedTime);

	// ずらした射撃方向を取得する
	DirectX::SimpleMath::Quaternion GetShotRotation();
};
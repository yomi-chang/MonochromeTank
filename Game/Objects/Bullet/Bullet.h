//#pragma once
//#include "Interface/IBullet.h"
//
//class Bullet : public IBullet
//{
//public:
//	// 砲弾モデルスケール
//	static const DirectX::SimpleMath::Vector3 BULLET_MODEL_SCALE;
//	// 砲弾境界ボックススケール
//	static const DirectX::SimpleMath::Vector3 BULLET_BOUNDINGBOX_SCALE;
//	// 境界(最小)
//	static const float MIN_BOUNDARY;
//	// 境界(最大)
//	static const float MAX_BOUNDARY;
//	// 砲弾スピードを定義する
//	static const DirectX::SimpleMath::Vector3 SPEED;
//	// 発射する力
//	static const float IMPULSIVE_FORCE;
//	// 質量
//	static const float MASS;
//	// 直径 (m)
//	static const float DIAMETER;
//	// 空気抵抗係数(Cd)
//	static const float DRAG_COEFFICIENT;
//	// 空気密度(ρ) kg/m^3
//	static const float AIR_DENSITY;
//	//サッカーボール直径断面積( m^2)
//	static const float CROSS_SECTION;
//	// 空気抵抗係数(k)
//	// ボールに働く空気抵抗はボールの速度の二乗に比例する
//	static const float AIR_RESISTANCE;
//
//public:
//	// 位置を取得する 
//	DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
//	// 位置を設定する
//	virtual void SetPosition(const DirectX::SimpleMath::Vector3& position) { m_position = position; };
//	// 反射力を取得する
//	float GetImpulsiveForce() const { return IMPULSIVE_FORCE; }
//	// 砲弾上下回転角を取得する 
//	float GetAngleUD() const { return m_angleUD; }
//	// 砲弾上下回転角を設定する 
//	void SetAngleUD(const float& angleUD) { m_angleUD = angleUD; }
//	// 砲弾左右回転角を取得する 
//	float GetAngleRL() const { return m_angleRL; }
//	// 砲弾左右回転角を設定する
//	void SetAngleRL(const float& angleRL) { m_angleRL = angleRL; }
//	// 砲弾が発射されているかどうかを取得する
//	IBullet::BulletState GetBulletState() const { return m_bulletState; }
//	// 砲弾の状態を設定する
//	void SetBulletState(IBullet::BulletState bulletState) { m_bulletState = bulletState; }
//	// モデルを取得する
//	DirectX::Model* GetModel() { return m_model; }
//	// ワールド境界ボックスを取得する
//	DirectX::BoundingBox* GetWorldBoundingBox() { return &m_worldBoundingBox; }
//	// ワールドマトリックスを取得する
//	DirectX::SimpleMath::Matrix& GetWorldMatrix() { return m_worldMatrix; }
//
//public:
//	// コンストラクタ
//	Bullet(int nodeNumber, IBullet::BulletState bulletState);
//	// デストラクタ
//	~Bullet();
//	// 初期化する
//	void Initialize();
//	// Bulletオブジェクトを更新する 
//	inline void Update(const float timer);
//	// Bulletオブジェクトを描画する 
//	inline void Render();
//	// 砲弾を描画する
//	inline void DrawBullet(const DirectX::SimpleMath::Matrix& matrix);
//	// 境界ボックスを描画する
//	inline void DrawBoundingBox(const DirectX::SimpleMath::Matrix& matrix);
//	// Bulletオブジェクトの後処理をおこなう 
//	void Finalize();
//
//private:
//	// グラフィックス
//	Graphics* m_graphics = Graphics::GetInstance();
//	// 「砲弾」モデル
//	DirectX::Model* m_model;
//	// 加速度
//	DirectX::SimpleMath::Vector3 m_acceralation;
//	// 速度
//	DirectX::SimpleMath::Vector3 m_velocity;
//	// 位置
//	DirectX::SimpleMath::Vector3 m_position;
//	// 上下角度
//	float m_angleUD;
//	// 左右角度
//	float m_angleRL;
//	// ワールド行列
//	DirectX::SimpleMath::Matrix m_worldMatrix;
//	// 「弾」境界ボックス
//	DirectX::BoundingBox m_boundingBox;
//	DirectX::BoundingBox m_worldBoundingBox;
//	// 砲弾が発射されているかどうかを示す
//	BulletState m_bulletState;
//};
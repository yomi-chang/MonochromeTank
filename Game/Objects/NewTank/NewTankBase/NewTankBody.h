#pragma once
#include "Interface/IObject.h"

class NewTank;

class NewTankBody : public IObject
{
public:
	// コンストラクタ
	NewTankBody(
		NewTank* tank,
		const DirectX::SimpleMath::Vector3& initialPosition,
		const float& initialAngle
	);
	// デストラクタ
	~NewTankBody() override;
	// 初期化処理
	void Initialize() override;
	// 更新処理
	void Update(float elapsedTime)override{}
	void Update(
		float elapsedTime,
		const DirectX::SimpleMath::Vector3& currentPosition,
		const DirectX::SimpleMath::Quaternion& currentAngle
	) override;
	// 描画処理
	void Render() override;
	// 終了処理
	void Finalize() override;

	// パーツ追加
	void Attach(std::unique_ptr<IObject> part) override;
	// パーツ削除
	void Detach(std::unique_ptr<IObject> part) override;

private:
	Graphics* m_graphics;									// グラフィックス
	DirectX::SimpleMath::Vector3 m_initialPosition;			// 初期座標
	DirectX::SimpleMath::Quaternion m_initialAngle;			// 初期回転角
	DirectX::SimpleMath::Vector3 m_currentPosition;			// 現在の座標
	DirectX::SimpleMath::Quaternion m_currentAngle;			// 現在の回転角
	std::vector<std::unique_ptr<IObject>> m_tankParts;		// 自身が管理する戦車部品の配列
	DirectX::SimpleMath::Matrix m_worldMatrix;				// ワールド行列
	DirectX::Model* m_model;								// モデル

	DirectX::SimpleMath::Quaternion m_bodyAngle;
	NewTank* m_tank;

	DirectX::SimpleMath::Vector3 m_collisionVel;

public:
	// 角度の取得
	DirectX::SimpleMath::Quaternion GetAngle(){ return m_currentAngle; }
	// 座標の取得
	DirectX::SimpleMath::Vector3 GetPosition() { return m_currentPosition; }

	// 押し戻しベクトルの設定
	void SetCollisionVel(DirectX::SimpleMath::Vector3 vel) 
	{
		// 押し戻しのベクトルが0なら処理を行わない
		if (vel == DirectX::SimpleMath::Vector3::Zero)
			return;

		m_currentPosition += vel;
	}

	// 座標の受け取り
	void SetPosition(DirectX::SimpleMath::Vector3 position) { m_currentPosition = position; }

	// 移動処理
	void Move(DirectX::SimpleMath::Vector3 velocity);

	// 回転処理
	void Rotate(DirectX::SimpleMath::Quaternion angle);
};
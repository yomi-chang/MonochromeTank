/*
	@file	TankBody.h
	@brief	車体クラス
*/
#pragma once
#include "Interface/IParts.h"

class Tank;

class TankBody : public IParts
{
public:
	// コンストラクタ
	TankBody(
		Tank* tank,
		const DirectX::SimpleMath::Vector3& initialPosition,
		const float& initialAngle
	);
	// デストラクタ
	~TankBody() override;
	// 初期化処理
	void Initialize() override;
	// 更新処理
	void Update(
		float elapsedTime,
		const DirectX::SimpleMath::Vector3& currentPosition,
		const DirectX::SimpleMath::Quaternion& currentRotation
	) override;
	// 描画処理
	void Render() override;
	// 終了処理
	void Finalize() override;

	// パーツ追加
	void Attach(std::unique_ptr<IParts> parts) override;
	// パーツ削除
	void Detach(std::unique_ptr<IParts> parts) override;

private:
	Graphics* m_graphics;									// グラフィックス
	DirectX::SimpleMath::Vector3 m_initialPosition;			// 初期座標
	DirectX::SimpleMath::Quaternion m_initialRotation;		// 初期回転角
	DirectX::SimpleMath::Vector3 m_currentPosition;			// 現在の座標
	DirectX::SimpleMath::Quaternion m_currentRotation;		// 現在の回転角
	std::vector<std::unique_ptr<IParts>> m_tankParts;		// 自身が管理する戦車部品の配列
	DirectX::SimpleMath::Matrix m_worldMatrix;				// ワールド行列
	DirectX::Model* m_model;								// モデル

	DirectX::SimpleMath::Quaternion m_bodyRotation;			// 車体の回転角
	Tank* m_tank;											// 親の情報
	bool m_gravityEnabled;									// 重力が有効かどうか

public:
	// 角度の取得
	DirectX::SimpleMath::Quaternion GetRotation(){ return m_currentRotation; }
	// 座標の取得
	DirectX::SimpleMath::Vector3 GetPosition() { return m_currentPosition; }

	// 押し戻しベクトルの設定
	void SetCollisionVel(DirectX::SimpleMath::Vector3 vel) { m_currentPosition += vel;}

	// 座標の受け取り
	void SetPosition(DirectX::SimpleMath::Vector3 position) { m_currentPosition = position; }

	// 移動処理
	void Move(DirectX::SimpleMath::Vector3 velocity);

	// 回転処理
	void Rotate(DirectX::SimpleMath::Quaternion rotation);

	void SetGravity(bool b) { m_gravityEnabled = b; }

private:
	// 影の描画
	//void DrawShadow();
};
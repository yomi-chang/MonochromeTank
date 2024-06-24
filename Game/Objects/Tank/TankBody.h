// 戦車の更新や描画などを担うクラス
#pragma once
#include "Interface/ITankComponent.h"
#include "Game/Objects/Tank/TankBase.h"

class TankBody : public TankBase
{
public:
	// コンストラクタ
	TankBody(
		ITankComponent* parent,
		const DirectX::SimpleMath::Vector3& initialPosition,
		const float& initialAngleRL,
		TankType type
	);

	// デストラクタ
	~TankBody() override;

	// 初期化処理
	void Initialize() override;

	// 更新処理
	void Update(
		float elapsedTime,
		const DirectX::SimpleMath::Vector3& currentPosition,
		const float& currentAngleRL
	) override;

	// 自身を描画しない描画処理
	void Render() override;

	// 終了処理
	void Finalize() override;

private:
	// グラフィックス
	Graphics* m_graphics;

	// 初期位置
	DirectX::SimpleMath::Vector3 m_currentPosition;

	// 初期回転角
	float m_currentAngleRL;

	// 自身が管理する戦車部品の配列
	std::vector<std::unique_ptr<ITankComponent>> m_tankParts;

	// モデル
	DirectX::Model* m_model;

	// ワールド行列
	DirectX::SimpleMath::Matrix m_worldMatrix;

	// 敵かプレイヤーか
	TankBase::TankType m_tankType;
};
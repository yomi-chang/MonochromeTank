// 戦車の更新や描画などを担うクラス
#pragma once
#include "Interface/IComponent.h"
#include "Interface/ILeaf.h"

class TankArmor : public ILeaf
{
public:
	// 親オブジェクトを取得する
	IComponent* GetParent() const { return m_parent; }

public:
	// コンストラクタ
	TankArmor(
		IComponent* parent,
		const DirectX::SimpleMath::Vector3& initialPosition,
		const float& initialAngleRL
	);

	// デストラクタ
	~TankArmor();

	// 初期化処理
	void Initialize(Type type);

	// 更新処理
	void Update(
		float elapsedTime,
		const DirectX::SimpleMath::Vector3& currentPosition,
		const float& currentAngleRL
	);

	// 自身を描画しない描画処理
	void Render();

	// 終了処理
	void Finalize();

private:
	// グラフィックス
	Graphics* m_graphics;

	// 親オブジェクト
	IComponent* m_parent;

	// 初期座標
	DirectX::SimpleMath::Vector3 m_initialPosition;

	// 初期回転角
	float m_initialAngle;

	// 現在の座標
	DirectX::SimpleMath::Vector3 m_currentPosition;

	// 現在の回転角
	float m_currentAngleRL;

	// 自身が管理する戦車部品の配列
	std::vector<std::unique_ptr<IComponent>> m_tankParts;

	// モデル
	//DirectX::Model* m_model;

	// ジオメトリックプリミティブ
	std::unique_ptr<DirectX::GeometricPrimitive> m_model;

	// ワールド行列
	DirectX::SimpleMath::Matrix m_worldMatrix;

	// 敵かプレイヤーか
	Type m_tankType;

	// カラー
	DirectX::SimpleMath::Vector4 m_color;
};
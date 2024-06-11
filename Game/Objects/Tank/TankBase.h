// 戦車の更新や描画などを担うクラス
#pragma once
#include "Interface/ITankComponent.h"
#include "Framework/Graphics.h"

class TankBase : public ITankComponent
{
public:
	// 初期位置の取得
	DirectX::SimpleMath::Vector3 GetInitialPosition() { return m_initialPosition; }

	//初期回転角の取得
	float GetInitialAngleRL() { return m_initialAngleRL; }
public:
	// コンストラクタ
	TankBase(
		ITankComponent* parent,
		const DirectX::SimpleMath::Vector3& initialPosition,
		const float& initialAngleRL
	);

	// デストラクタ
	virtual ~TankBase();

	// 初期化処理
	virtual void Initialize();

	// モデルのセット
	virtual void SetModel(DirectX::Model* model);

	// 更新処理
	virtual void Update(
		float elapsedTime,
		const DirectX::SimpleMath::Vector3& currentPosition,
		const float& currentAngleRL
	);

	// 自身を描画しない描画処理
	virtual void Render();

	// 描画処理
	virtual void Render(const DirectX::SimpleMath::Matrix& matrix);

	// 部品の追加
	virtual void Attach(std::unique_ptr<ITankComponent> tankPart);

private:
	// グラフィックス
	Graphics* m_graphics;

	// 親の情報
	ITankComponent* m_parent;

	// 初期位置
	DirectX::SimpleMath::Vector3 m_initialPosition;

	// 初期回転角
	float m_initialAngleRL;

	// 自身が管理する戦車部品の配列
	std::vector<std::unique_ptr<ITankComponent>> m_tankParts;

	// パーツモデル
	DirectX::Model* m_model;
};
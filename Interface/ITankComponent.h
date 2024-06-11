#pragma once

class ITankComponent
{
public:
	//デストラクタ
	virtual ~ITankComponent() = default;

	//初期化処理
	virtual void Initialize() = 0;

	//更新処理
	virtual void Update(
		float elapsedTime,
		const DirectX::SimpleMath::Vector3& currentPosition,
		const float& currentAngleRL
	) = 0;

	//描画処理
	virtual void Render() = 0;

	//終了処理
	virtual void Finalize() = 0;

	// 砲塔部品を追加する
	virtual void Attach(std::unique_ptr<ITankComponent> tankParts) = 0;
};

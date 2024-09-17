#pragma once
#include "Framework/Graphics.h"

class IComponent
{
public:
	enum Type
	{
		PLAYER,
		ENEMY
	};

public:
	// 親を取得する
	virtual IComponent* GetParent() const = 0;

public:
	//デストラクタ
	virtual ~IComponent() = default;

	//初期化処理
	virtual void Initialize(
		Type type
	) = 0;

	//更新処理
	virtual void Update(
		float elapsedTime,
		const DirectX::SimpleMath::Vector3& currentPosition,
		const DirectX::SimpleMath::Quaternion& currentAngle
	) = 0;

	//描画処理
	virtual void Render() = 0;

	//終了処理
	virtual void Finalize() = 0;
};

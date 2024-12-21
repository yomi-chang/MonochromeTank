#pragma once
#include "Framework/Graphics.h"
#include "Framework/Resources.h"
#include "Libraries/MyLib/DebugLog.h"

class IParts
{
public:
	// デストラクタ
	virtual ~IParts() = default;
	// 初期化処理
	virtual void Initialize() = 0;
	// 更新処理
	virtual void Update(
		float elapsedTime,
		const DirectX::SimpleMath::Vector3& currentPosition,
		const DirectX::SimpleMath::Quaternion& currentRotation
	) = 0;
	// 描画処理
	virtual void Render() = 0;
	// 終了処理
	virtual void Finalize() = 0;

	// 追加
	virtual void Attach(std::unique_ptr<IParts> parts) = 0;
	// 削除
	virtual void Detach(std::unique_ptr<IParts> parts) = 0;
};
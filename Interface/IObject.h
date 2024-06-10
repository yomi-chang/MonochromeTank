#pragma once

class IObject
{
public:
	//デストラクタ
	virtual ~IObject() = default;

	//初期化処理
	virtual void Initialize() = 0;

	//更新処理
	virtual void Update() = 0;

	//描画処理
	virtual void Render() = 0;

	//終了処理
	virtual void Finalize() = 0;
};

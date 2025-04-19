/*
	@file	IParent.h
	@brief	Parentインターフェース
*/
#pragma once
#include "Framework/Graphics.h"
#include "Framework/Resources.h"
#include "Interface/IParts.h"

class IParent
{
public:
	// デストラクタ
	virtual ~IParent() = default;
	// 初期化処理
	virtual void Initialize() = 0;
	// 更新処理
	virtual void Update(float elapsedTime) = 0;
	// 描画処理
	virtual void Render() = 0;
	// 終了処理
	virtual void Finalize() = 0;

	// 部品の追加
	virtual void Attach(std::unique_ptr<IParts> parts) = 0;
	// 部品の削除
	virtual void Detach(std::unique_ptr<IParts> parts) = 0;
};
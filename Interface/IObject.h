/*
	@file	IObject.h
	@brief	Objectインターフェース
*/
#pragma once
#include "Framework/Graphics.h"
#include "Framework/Resources.h"
#include "Libraries/MyLib/DebugLog.h"

class IObject
{
public:
	// デストラクタ
	virtual ~IObject() = default;
	// 初期化処理
	virtual void Initialize() = 0;
	// 更新処理
	virtual void Update(float elapsedTime) = 0;
	// 描画処理
	virtual void Render() = 0;
	// 終了処理
	virtual void Finalize() = 0;
};
/*
	@file	IEnemyAction.h
	@brief	EnemyActionインターフェース
*/
#pragma once
#include "Libraries/MyLib/DebugLog.h"
#include "Game/Objects/Tank/TankBase/Tank.h"
#include "Libraries/MyLib/Math.h"
#include "Game/Other/Parameter.h"

class IEnemyAction
{
public:
	// デストラクタ
	virtual ~IEnemyAction() = default;
	// 初期化処理
	virtual void Initialize(Tank* tank) = 0;
	// 更新処理
	virtual void Update(float elapsedTime) = 0;
};
/*
	@file	IState.h
	@brief	IStateインターフェース
*/
#pragma once
#include "Libraries/MyLib/DebugLog.h"
#include "Game/Objects/Tank/TankBase/Tank.h"
#include "Libraries/MyLib/Utils.h"
#include "Game/Other/Parameter.h"
#include "Message/Messenger.h"

class IState
{
public:
	// デストラクタ
	virtual ~IState() = default;
	// 初期化処理
	virtual void Initialize(Tank* tank) = 0;
	// 更新処理
	virtual void Update(float elapsedTime) = 0;
	// 追跡対象の戦車の取得
	virtual Tank* GetTargetTank() = 0;
	// 追跡対象の戦車の設定
	virtual void SetTargetTank(Tank* targetTank) = 0;
};